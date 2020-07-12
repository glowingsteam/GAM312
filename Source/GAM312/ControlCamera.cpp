// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlCamera.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AControlCamera::AControlCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AControlCamera::BeginPlay()
{
	Super::BeginPlay();

	// Get Player Controller
	OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
}

// Called every frame
void AControlCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check for PC Validity
	if (OurPlayerController)
	{
		// Update Time
		timeToNextCameraChange -= DeltaTime;

		// Debug Time
		//UE_LOG(LogTemp, Warning, TEXT("Time to Change: %f"), timeToNextCameraChange);

		// Check if timeout was reached
		if (timeToNextCameraChange <= 0.0f)
		{
			// Reset timer
			timeToNextCameraChange = timeBetweenCameraChange;

			// If CameraTwo Valid && Viewing CameraOne
			if (CameraTwo && (OurPlayerController->GetViewTarget() == CameraOne))
			{
				OurPlayerController->SetViewTargetWithBlend(CameraTwo, smoothBlend);
			}

			// If CameraOne Valid
			else if (CameraOne)
			{
				OurPlayerController->SetViewTargetWithBlend(CameraOne, smoothBlend);
			}
		}
	}
}

