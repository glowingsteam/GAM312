// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GAM312Character.h"
#include "GAM312Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Kismet/GameplayStatics.h"
#include "iface\InteractionInterface.h"
#include "WorldObjects\PhysicsPickupActor.h"
#include "Engine.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AGAM312Character

AGAM312Character::AGAM312Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Make Physics attach location
	PhysicsAttachLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Physics Attach Location"));
	PhysicsAttachLocation->SetupAttachment(Mesh1P);
	PhysicsAttachLocation->SetRelativeLocation(FVector(400.0f, 0.0f, 0.0f));

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	Beam = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GrappleBeam"));
	Beam->SetupAttachment(Mesh1P, TEXT("hand_l"));
	Beam->SetVisibility(false, false);
}

void AGAM312Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}

	// Set our player controller ref
	OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);

	// Set current health to max
	currentHealth = maxHealth;

	// Set grapple mana to max
	currentGrappleMana = maxGrappleMana;
}

void AGAM312Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// The grappling mechanic utilizes Linear Algebra concepts by smoothly lerping between the players current velocity and a generated vector based on the direction to the grapple location * a scalar value
	if (grappling)
	{
		// Check if we are within range of break off distance and end grapple
		if (FVector::Distance(GetActorLocation(),  grappleToLocation) < grappleBreakOffDistance)
		{
			EndGrapple();
		}
		
		else
		{
			// else lerp velocity to get to location
			GetCharacterMovement()->Velocity = FMath::Lerp(GetCharacterMovement()->Velocity, (grappleToLocation - GetActorLocation()) * grappleSpeed, lerpSpeed);
			Beam->SetBeamSourcePoint(0, FP_Gun->GetComponentLocation(), 0);
			Beam->SetBeamTargetPoint(0, grappleToLocation, 0);

			// Drain Mana & Break if less than zero
			currentGrappleMana = currentGrappleMana - (grappleManaDrainRate * DeltaTime);
			UE_LOG(LogTemp, Warning, TEXT("Current Mana: %f"), currentGrappleMana);
			if (currentGrappleMana <= 0.0f)
			{
				EndGrapple();
			}
		}
	}
	// Grapple Mana recovery
	else
	{
		currentGrappleMana = FMath::Clamp(currentGrappleMana + (grappleManaRecoveryRate * DeltaTime), 0.0f, maxGrappleMana);
	}

	if (pickingUpObject )
	{
		pickedUpComp->SetWorldLocation(PhysicsAttachLocation->GetComponentLocation());
		pickedUpComp->SetWorldRotation(PhysicsAttachLocation->GetComponentRotation());
		//UE_LOG(LogTemp, Warning, TEXT("Picked up Object Location - X: %f Y: %f Z: %f"), pickedUpObject->GetActorLocation().X, pickedUpObject->GetActorLocation().Y, pickedUpObject->GetActorLocation().Z);
	}
}

float AGAM312Character::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	// Save super return value for output
	float superReturn = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Process Damage
	currentHealth = FMath::Clamp(currentHealth - DamageAmount, -1.0f, 100.0f);

	// Check if Dead
	if (currentHealth <= 0.0f)
	{
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
		UE_LOG(LogTemp, Warning, TEXT("Dead"));
	}

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("Current Health: %f"), currentHealth);

	// Return super value
	return superReturn;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGAM312Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AGAM312Character::OnFire);

	// SecurityCamera Controller
	PlayerInputComponent->BindAction("SwitchCameras", IE_Pressed, this, &AGAM312Character::SwitchCameras);
	PlayerInputComponent->BindAction("ExitCameraView", IE_Pressed, this, &AGAM312Character::ExitCameraView);

	// Raycast
	PlayerInputComponent->BindAction("Raycast", IE_Pressed, this, &AGAM312Character::DisplayRaycast);
	
	// Grapple
	PlayerInputComponent->BindAction("Grapple", IE_Pressed, this, &AGAM312Character::BeginGrapple);
	PlayerInputComponent->BindAction("Grapple", IE_Released, this, &AGAM312Character::EndGrapple);

	// Pickup
	PlayerInputComponent->BindAction("Pickup", IE_Pressed, this, &AGAM312Character::PushPickupPhysicsObject);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AGAM312Character::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AGAM312Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGAM312Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGAM312Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGAM312Character::LookUpAtRate);

	
}

void AGAM312Character::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<AGAM312Projectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<AGAM312Projectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AGAM312Character::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AGAM312Character::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AGAM312Character::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AGAM312Character::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AGAM312Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AGAM312Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AGAM312Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGAM312Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AGAM312Character::SwitchCameras()
{
	// Check if we are not  viewing a camera
	if (!viewingCamera)
	{
		// See if we are over bounds of array
		if (allCameras.IsValidIndex(currentCameraIndex))
		{
			// Switch over to camera and set viewing camera to true
			OurPlayerController->SetViewTargetWithBlend(allCameras[currentCameraIndex]);
			viewingCamera = true;
		}

		// If we were over bounds, make sure that there is a valid camera initialized in the array
		else if (allCameras.IsValidIndex(0))
		{
			// Reset index and switch over to viewing camera
			currentCameraIndex = 0;
			OurPlayerController->SetViewTargetWithBlend(allCameras[currentCameraIndex]);
			viewingCamera = true;
		}
	}

	// Check if we are already viewing a camera
	if (viewingCamera)
	{
		// Increment index
		currentCameraIndex++;

		// Check if index is valid and switch cameras
		if (allCameras.IsValidIndex(currentCameraIndex))
		{
			OurPlayerController->SetViewTargetWithBlend(allCameras[currentCameraIndex], smoothBlend);
		}

		// If previous index was not valid, check if array is valid and switch camera
		else if (allCameras.IsValidIndex(0))
		{
			currentCameraIndex = 0;
			OurPlayerController->SetViewTargetWithBlend(allCameras[currentCameraIndex], smoothBlend);
		}
	}
}

void AGAM312Character::ExitCameraView()
{
	// If viewing camera then switch back to player
	if (viewingCamera)
	{
		OurPlayerController->SetViewTargetWithBlend(this, smoothBlend);
		viewingCamera = false;
	}
}

void AGAM312Character::DisplayRaycast()
{
	FHitResult* hit = new FHitResult();
	FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	FVector EndTrace = ((ForwardVector * 3319.0f) + StartTrace);
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();

	if (GetWorld()->LineTraceSingleByChannel(*hit, StartTrace, EndTrace, ECC_Visibility, *TraceParams))
	{
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 0), true);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("You hit: %s"), *hit->Actor->GetName()));
	}
}

void AGAM312Character::BeginGrapple()
{
	// If not grappling and current mana is greater than minimum
	if (!grappling && currentGrappleMana >= grappleMinimum && !pickingUpObject)
	{
		FHitResult* hit = new FHitResult();
		FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
		FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
		FVector EndTrace = ((ForwardVector * grappleTraceDistance) + StartTrace);
		FCollisionQueryParams* TraceParams = new FCollisionQueryParams();

		if (GetWorld()->LineTraceSingleByChannel(*hit, StartTrace, EndTrace, ECC_Visibility, *TraceParams))
		{
			if (hit->bBlockingHit)
			{
				Beam->SetVisibility(true, false);
				Beam->SetBeamSourcePoint(0, FP_Gun->GetComponentLocation(), 0);
				Beam->SetBeamTargetPoint(0, hit->ImpactPoint, 0);
				grappleToLocation = hit->ImpactPoint;
				GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
				grappling = true;
			}
		}
	}
}

void AGAM312Character::EndGrapple()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	grappling = false;
	Beam->SetVisibility(false, false);
}

void AGAM312Character::PushPickupPhysicsObject()
{
	// Ensure we are not grappling
	if (!grappling)
	{
		// Check if we are already picking up object and the object is not a nullptr
		if (pickingUpObject && pickedUpObject != nullptr)
		{
			// Get pointer and call interface msg
			IInteractionInterface* pointerToInterface = Cast<IInteractionInterface>(pickedUpObject);
			if (pointerToInterface != nullptr)
			{
				pointerToInterface->Execute_ReleaseCube(pickedUpObject);
			}

			// Reset vars
			pickingUpObject = false;
			pickedUpObject = nullptr;
		}

		// Otherwise find a new object to trace to
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Starting Trace"));
			// Setup vars to trace for object
			FHitResult* hit = new FHitResult();
			FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
			FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
			FVector EndTrace = ((ForwardVector * 1000.0f) + StartTrace);
			FCollisionQueryParams* TraceParams = new FCollisionQueryParams();

			// Trace for object
			if (GetWorld()->LineTraceSingleByChannel(*hit, StartTrace, EndTrace, ECC_Visibility, *TraceParams))
			{
				if (hit->Actor->ActorHasTag(FName("Pickup")))
				//if (true)
				{
					// Get pointer and call interface msg
					IInteractionInterface* pointerToInterface = Cast<IInteractionInterface>(hit->Actor);
					if (pointerToInterface != nullptr)
					{
						// Set pick up object and save reference to mesh for tick
						pickingUpObject = true;
						pickedUpObject = hit->GetActor();
						pickedUpComp = Cast<APhysicsPickupActor>(hit->GetActor())->MeshComponent;
						pointerToInterface->Execute_PickupCube(pickedUpObject);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("No Interface"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("No Tag"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit Nothing"));
			}
		}
	}
}

bool AGAM312Character::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AGAM312Character::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AGAM312Character::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AGAM312Character::TouchUpdate);
		return true;
	}
	
	return false;
}
