// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicsPickupActor.h"

// Sets default values
APhysicsPickupActor::APhysicsPickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root guhponent
	rootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = rootSceneComp;

	// Create a Static Mesh	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshBase"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	MeshComponent->SetSimulatePhysics(true);

}

// Called when the game starts or when spawned
void APhysicsPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APhysicsPickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APhysicsPickupActor::PickupCube_Implementation()
{
	// When picked up disable physics
	MeshComponent->SetSimulatePhysics(false);
	beingPickedUp = true;
}

void APhysicsPickupActor::ReleaseCube_Implementation()
{
	// When dropped, re-enable physics
	MeshComponent->SetSimulatePhysics(true);
	beingPickedUp = false;
}

void APhysicsPickupActor::TriggerActor_Implementation()
{
}

void APhysicsPickupActor::UntriggerActor_Implementation()
{
}

