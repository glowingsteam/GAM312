// Fill out your copyright notice in the Description page of Project Settings.


#include "SecurityCamera.h"

ASecurityCamera::ASecurityCamera()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASecurityCamera::BeginPlay()
{
	Super::BeginPlay();
}

void ASecurityCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}