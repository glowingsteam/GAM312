// Fill out your copyright notice in the Description page of Project Settings.


#include "AntiGravStaticMeshActor.h"

// Sets default values
AAntiGravStaticMeshActor::AAntiGravStaticMeshActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	rootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = rootSceneComp;

	// Create a Static Mesh	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshBase"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("Pawn"));
	MeshComponent->SetSimulatePhysics(true);
}

// Called when the game starts or when spawned
void AAntiGravStaticMeshActor::BeginPlay()
{
	Super::BeginPlay();
	
	ReenableGravityDelegate.BindUFunction(this, FName("EnableGravity"));
}

// Called every frame
void AAntiGravStaticMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAntiGravStaticMeshActor::TriggerAntiGravMechanic_Implementation()
{
	MeshComponent->SetEnableGravity(false);
	if (GetWorldTimerManager().IsTimerActive(ReenableGravityTimer)) { GetWorld()->GetTimerManager().ClearTimer(ReenableGravityTimer); }
	GetWorldTimerManager().SetTimer(ReenableGravityTimer, ReenableGravityDelegate, 10.0f, false, floatDuration);
	MeshComponent->AddImpulseAtLocation(FVector(0.0f,0.0f, moveUpAmount), GetActorLocation());
	//UE_LOG(LogTemp, Warning, TEXT("Interacted With"));
}

void AAntiGravStaticMeshActor::EnableGravity()
{
	MeshComponent->SetEnableGravity(true);
}

