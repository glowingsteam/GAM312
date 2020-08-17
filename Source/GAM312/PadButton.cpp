// Fill out your copyright notice in the Description page of Project Settings.


#include "PadButton.h"

// Sets default values
APadButton::APadButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create root component
	rootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = rootSceneComp;

	// Init overlap
	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Box Component"));
	OverlapBox->InitBoxExtent(FVector(10.0f));
	OverlapBox->SetCollisionProfileName(TEXT("Trigger"));
	OverlapBox->SetupAttachment(RootComponent);
	OverlapBox->bMultiBodyOverlap = true;

	// Bind overlap event
	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &APadButton::OnOverlapBegin);
	OverlapBox->OnComponentEndOverlap.AddDynamic(this, &APadButton::OnOverlapEnd);
}

// Called when the game starts or when spawned
void APadButton::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APadButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APadButton::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Check if other actor has cube to continue functionality
	if (OtherActor->ActorHasTag(FName("Cube")))
	{
		// Iterate through triggerables
		for (int i = 0; i < actorsToTrigger.Num(); i++)
		{
			// Get pointer and call interface msg
			IInteractionInterface* pointerToInterface = Cast<IInteractionInterface>(actorsToTrigger[i]);
			if (pointerToInterface != nullptr)
			{
				pointerToInterface->Execute_TriggerActor(actorsToTrigger[i]);
			}
		}
	}
}

void APadButton::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{

	// Check if other actor has cube to continue functionality
	if (OtherActor->ActorHasTag(FName("Cube")))
	{
		// Iterate through triggerables
		for (int i = 0; i < actorsToTrigger.Num(); i++)
		{
			// Get pointer and call interface msg
			IInteractionInterface* pointerToInterface = Cast<IInteractionInterface>(actorsToTrigger[i]);
			if (pointerToInterface != nullptr)
			{
				pointerToInterface->Execute_UntriggerActor(actorsToTrigger[i]);
			}
		}
	}
}

