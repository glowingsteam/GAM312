// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "GAM312Character.h"
#include "MedKit.generated.h"

UCLASS()
class GAM312_API AMedKit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMedKit();

	/** Root Guhponent */
	UPROPERTY(EditAnywhere)
		USceneComponent* rootSceneComp;

	/** Overlap Component */
	UPROPERTY(VisibleAnywhere, Category = "Overlap")
		class UBoxComponent* OverlapBox;

	/** Overlap Component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float healAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		FDamageEvent damageEvent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// declare overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
