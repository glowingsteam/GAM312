// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "SecurityCamera.generated.h"

/**
 * 
 */
UCLASS()
class GAM312_API ASecurityCamera : public ACameraActor
{
	GENERATED_BODY()
	
public:
	ASecurityCamera();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
