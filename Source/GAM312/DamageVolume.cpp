// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageVolume.h"

// Sets default values
ADamageVolume::ADamageVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create root component
	rootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = rootSceneComp;

	// Init overlap
	DamageBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Box Component"));
	DamageBox->InitBoxExtent(DamageBounds);
	DamageBox->SetCollisionProfileName(TEXT("Trigger"));
	DamageBox->SetupAttachment(RootComponent);
	DamageBox->bMultiBodyOverlap = true;

	
}

// Called when the game starts or when spawned
void ADamageVolume::BeginPlay()
{
	Super::BeginPlay();
	// Setup Damage Timer
	DamageTimerDel.BindUFunction(this, FName("DamageOverlappingTargets"));
	if (GetWorldTimerManager().IsTimerActive(DamageTimer)) { GetWorld()->GetTimerManager().ClearTimer(DamageTimer); }
	GetWorldTimerManager().SetTimer(DamageTimer, DamageTimerDel, DamageTickRate, true);
}

// Called every frame
void ADamageVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADamageVolume::DamageOverlappingTargets()
{
	// Get all overlapping Actors
	TArray<AActor*> overlappingActors;
	DamageBox->GetOverlappingActors(overlappingActors);

	// Check if any actors are overlapped
	if (overlappingActors.Num() > 0)
	{
		// Iterate through loop
		for (int i = 0; i < overlappingActors.Num(); i++)
		{
			// Check overlapped actor validity
			if (overlappingActors[i] != nullptr)
			{
				// Apply Damage
				overlappingActors[i]->TakeDamage(DamageAmount * DamageTickRate, damageEvent, nullptr, this);
			}
		}
	}
}

