// Fill out your copyright notice in the Description page of Project Settings.


#include "Triggerable.h"

// Sets default values
ATriggerable::ATriggerable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATriggerable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATriggerable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Overrideable functions for BP Implementation
void ATriggerable::TriggerActor_Implementation()
{
}

void ATriggerable::UntriggerActor_Implementation()
{
}

