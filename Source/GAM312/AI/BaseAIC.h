// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AI\BaseAI.h"
#include "AIStatusMode.h"
#include "AI\PatrolLocation.h"
#include "Kismet/GameplayStatics.h"
#include "BaseAIC.generated.h"

/**
 * 
 */
UCLASS()
class GAM312_API ABaseAIC : public AAIController
{
	GENERATED_BODY()
	
public:
	// BeginPlay
	void BeginPlay() override;
	
	// Movement Completed
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	// Patrol Move to
	void GotoNextWaypoint();

	// Checks for a single valid waypoint and sets canPatrol
	void CheckWaypointValidity();

	// Increments patrol index and will reset array at 0 if out of bounds
	void IncrementWaypointIndex();

	// Checks to see if we can see player
	UFUNCTION()
	void CheckForPlayerVisibility();

public:
	// Pathfind Accept Radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	float patrolAcceptanceRadius = 50;

	// Implement AI current status
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status)
	EAIStatusMode status = EAIStatusMode::Patrolling;

	// Controlled Pawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	AActor* controlledAI = nullptr;

	// Timer Handle
	FTimerHandle checkForPlayerTimer;
	FTimerDelegate checkForPlayerDel;
	// Timer frequency
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	float timerTick = 0.1f;

	// Player last location
	UPROPERTY()
	FVector playerLastLocation;

	// Player visibility iterator counter
	int visibilityChecks;

	// Max checks before lockout
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		int maxChecks = 50;

private:
	// Patrol Location Array
	UPROPERTY()
	TArray<APatrolLocation*> patrolLocations;

	// Current index of patrol point
	int currentPatrolLocation = 0;

	// Array to see if points are valid so endless loop does not occur
	bool canPatrol;
};
