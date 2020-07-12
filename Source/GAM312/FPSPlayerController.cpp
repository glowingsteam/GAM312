// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPlayerController.h"

AFPSPlayerController::AFPSPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AFPSPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}