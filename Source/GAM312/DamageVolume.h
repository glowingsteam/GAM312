// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "DamageVolume.generated.h"

UCLASS()
class GAM312_API ADamageVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamageVolume();

	/** Root Guhponent */
	UPROPERTY(EditAnywhere)
	USceneComponent* rootSceneComp;

	/** Overlap Component */
	UPROPERTY(VisibleAnywhere, Category = "Overlap")
	class UBoxComponent* DamageBox;

	/** Overlap Component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlap")
	FVector DamageBounds = FVector(10.0f);

	/** Damage Timers*/
	FTimerDelegate DamageTimerDel;
	FTimerHandle DamageTimer;

	/** Damage Vars */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float DamageTickRate = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float DamageAmount = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		FDamageEvent damageEvent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void DamageOverlappingTargets();
};
