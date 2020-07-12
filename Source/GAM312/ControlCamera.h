// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ControlCamera.generated.h"

UCLASS()
class GAM312_API AControlCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AControlCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = Cameras)
		AActor* CameraOne;

	UPROPERTY(EditAnywhere, Category = Cameras)
		AActor* CameraTwo;

	UPROPERTY()
		APlayerController* OurPlayerController;

	UPROPERTY(EditAnywhere, Category = Cameras)
	float timeBetweenCameraChange = 2.0f;

	UPROPERTY(EditAnywhere, Category = Cameras)
	float smoothBlend = 0.75f;

	float timeToNextCameraChange;

};
