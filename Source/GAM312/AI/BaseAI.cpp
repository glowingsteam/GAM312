// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAI.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine.h"

// Sets default values
ABaseAI::ABaseAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set movement speed to max patrol speed
	GetCharacterMovement()->MaxWalkSpeed = patrolMovementSpeed;
}

// Called when the game starts or when spawned
void ABaseAI::BeginPlay()
{
	Super::BeginPlay();
	// Get player reference
	playerRef = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

// Called every frame
void ABaseAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseAI::SetMovementSpeed(EAIStatusMode newMode)
{
	switch (newMode)
	{
	case EAIStatusMode::Patrolling:
		GetCharacterMovement()->MaxWalkSpeed = patrolMovementSpeed;
		break;
	case EAIStatusMode::SeePlayer:
		GetCharacterMovement()->MaxWalkSpeed = aggroMovementSpeed;
		break;
	case EAIStatusMode::LookForPlayer:
		GetCharacterMovement()->MaxWalkSpeed = aggroMovementSpeed;
		break;
	}

	UE_LOG(LogTemp, Warning, TEXT("Movement Speed: %f"), GetCharacterMovement()->MaxWalkSpeed);
}

TArray<APatrolLocation*> ABaseAI::GetPatrolLocations()
{
	return patrolPoints;
}

bool ABaseAI::CheckPlayerVisibility()
{
	if (playerRef != nullptr)
	{
		// Check pawn dot to player
		float dotProd = FVector::DotProduct((playerRef->GetActorLocation() - GetActorLocation()).GetSafeNormal(1.0f), GetActorForwardVector());

		// Pawn is looking at player
		if (playerTraceDot < dotProd)
		{
			// Trace for player
			FHitResult hit;
			FVector StartTrace = GetActorLocation();
			// Get forward vector to player if dot check passed and trace set distance
			FVector EndTrace = GetActorLocation() + ((playerRef->GetActorLocation() - GetActorLocation()).GetSafeNormal(1.0f) * playerTraceDistance);
			FCollisionQueryParams* TraceParams = new FCollisionQueryParams();

			// Ignore self in trace
			TArray <AActor*> actorsToIgnore;
			actorsToIgnore.Add(this);

			// If trace was successful
			if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), StartTrace, EndTrace, objTypes, false, actorsToIgnore, EDrawDebugTrace::None, hit, true, FLinearColor::Red, FLinearColor::Green, 5.0f))
			{
				//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 0), true);
				// If we hit player return true
				if (hit.Actor == playerRef)
				{
					return true;
				}

				// If no hit or did not hit player return false
				else
				{
					return false;
				}
			}

			// If trace unsuccessful return false
			else
			{
				return false;
			}
		}

		// Pawn not looking at player
		else
		{
			return false;
		}
	}

	// Player is nullptr
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerRefNotValid"));
		return false;
	}
}
