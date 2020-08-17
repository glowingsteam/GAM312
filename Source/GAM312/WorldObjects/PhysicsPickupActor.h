// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "iface/InteractionInterface.h"
#include "PhysicsPickupActor.generated.h"

UCLASS()
class GAM312_API APhysicsPickupActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APhysicsPickupActor();
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
		bool beingPickedUp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PickupCube_Implementation() override;

	virtual void ReleaseCube_Implementation() override;

	virtual void TriggerActor_Implementation() override;

	virtual void UntriggerActor_Implementation() override;
};
