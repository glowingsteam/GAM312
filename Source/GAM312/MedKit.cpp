// Fill out your copyright notice in the Description page of Project Settings.


#include "MedKit.h"

// Sets default values
AMedKit::AMedKit()
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
	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AMedKit::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AMedKit::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMedKit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMedKit::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AGAM312Character* playerRef = Cast<AGAM312Character>(OtherActor);
	if (playerRef)
	{
		playerRef->TakeDamage(healAmount * -1.0f, damageEvent, nullptr, this);
		Destroy();
	}

}

