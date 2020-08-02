// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AI\PatrolLocation.h"
#include "AIStatusMode.h"
//#include "AI\BaseAIC.h"
#include "BaseAI.generated.h"

UCLASS()
class GAM312_API ABaseAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseAI();

public:
	// Array of patrol locations for when patrolling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
		TArray<APatrolLocation*> patrolPoints;

	// Patrol movement speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
		float patrolMovementSpeed = 400;

	// Track player movement speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
		float aggroMovementSpeed = 600;

	// Player Visible Trace Distance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	float playerTraceDistance = 2000;

	// Player Trace dot product lockout
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	float playerTraceDot = 0.8f;

	// Player Reference
	UPROPERTY()
	AActor* playerRef;

	// Trace Helper Collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	TArray <TEnumAsByte<EObjectTypeQuery>> objTypes;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	// Set movement speed based on enum
	void SetMovementSpeed(EAIStatusMode newMode);

	// Get patrol points for AIC
	TArray<APatrolLocation*> GetPatrolLocations();

	// Check for player trace
	bool CheckPlayerVisibility();
};
