// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "iface\InteractionInterface.h"
#include "PadButton.generated.h"

UCLASS()
class GAM312_API APadButton : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APadButton();

	/** Root Guhponent */
	UPROPERTY(EditAnywhere)
		USceneComponent* rootSceneComp;

	/** Overlap Component */
	UPROPERTY(VisibleAnywhere, Category = "Overlap")
		class UBoxComponent* OverlapBox;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triggers")
	TArray<AActor*> actorsToTrigger;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// declare overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// declare overlap begin function
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
