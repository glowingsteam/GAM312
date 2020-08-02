// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "iface\ProjectileInteraction.h"
#include "TimerManager.h"
#include "AntiGravStaticMeshActor.generated.h"

UCLASS()
class GAM312_API AAntiGravStaticMeshActor : public AActor, public IProjectileInteraction
{
	GENERATED_BODY()
	
	
public:	
	// Sets default values for this actor's properties
	AAntiGravStaticMeshActor();

	/** Root Guhponent */
	UPROPERTY(EditAnywhere)
		USceneComponent* rootSceneComp;

	/** Static Mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class UStaticMeshComponent* MeshComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Projectile Interaction function
	virtual void TriggerAntiGravMechanic_Implementation() override;

	UFUNCTION()
	void EnableGravity();

	// How high to float up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	float moveUpAmount = 100.0f;

	// When to start floating back down
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	float floatDuration = 3.0f;

	FTimerDelegate ReenableGravityDelegate;
	FTimerHandle ReenableGravityTimer;

};
