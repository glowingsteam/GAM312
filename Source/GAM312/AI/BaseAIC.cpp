// Fill out your copyright notice in the Description page of Project Settings.
#include "BaseAIC.h"
#include "TimerManager.h"


void ABaseAIC::BeginPlay()
{
	Super::BeginPlay();
	// Get controlled character for updates
	controlledAI = Cast<AActor>(GetPawn());
	
	// Intialize Pawn
	if (controlledAI != nullptr)
	{
		patrolLocations = Cast<ABaseAI>(controlledAI)->GetPatrolLocations();
		Cast<ABaseAI>(controlledAI)->SetMovementSpeed(status);
		CheckWaypointValidity();
		GotoNextWaypoint();
	}

	// Bind Timer function
	checkForPlayerDel.BindUFunction(this, FName("CheckForPlayerVisibility"));
	GetWorldTimerManager().SetTimer(checkForPlayerTimer, checkForPlayerDel, timerTick, true, 0.5f);
}

void ABaseAIC::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult & Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	UE_LOG(LogTemp, Warning, TEXT("Move Completed"));
	GotoNextWaypoint();
}

void ABaseAIC::GotoNextWaypoint()
{
	// Check if pawn is valid and a single valid patrol point exists
	if (controlledAI != nullptr && canPatrol)
	{
		// Check if current index is valid, otherwise increment
		do { IncrementWaypointIndex();} 
		while (patrolLocations[currentPatrolLocation] == nullptr);

		// Move Actor to next patrol point
		MoveToActor(patrolLocations[currentPatrolLocation]);
		UE_LOG(LogTemp, Warning, TEXT("Patrolling to index: %d"), currentPatrolLocation);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Patrol"));
	}
}

void ABaseAIC::CheckWaypointValidity()
{
	if (patrolLocations.Num() > 0)
	{
		// Create temp var
		int invalidPoints = 0;

		// Iterate through points and count nullptr
		for (int i = 0; i < patrolLocations.Num(); i++)
		{
			if (patrolLocations[i] == nullptr)
			{
				invalidPoints++;
			}
		}

		// If all points are nullptr
		if (invalidPoints == patrolLocations.Num())
		{
			canPatrol = false;
			UE_LOG(LogTemp, Warning, TEXT("All points invalid"));
		}

		// If at least 1 point was valid
		else
		{
			canPatrol = true;
			UE_LOG(LogTemp, Warning, TEXT("A valid point exists"));
		}
	}

	// Array empty
	else
	{
		canPatrol = false;
		UE_LOG(LogTemp, Warning, TEXT("Patrol Array Empty"));
	}
}

void ABaseAIC::IncrementWaypointIndex()
{
	if (currentPatrolLocation + 1 == patrolLocations.Num())
	{
		currentPatrolLocation = 0;
	}
	else
	{
		currentPatrolLocation++;
	}
}

void ABaseAIC::CheckForPlayerVisibility()
{
	// Check if we can see player
	bool canSeePlayer = Cast<ABaseAI>(controlledAI)->CheckPlayerVisibility();

	// If AI is patrolling
	if (status == EAIStatusMode::Patrolling)
	{
		if (canSeePlayer)
		{
			// If we see player then chase player and save its last location
			status = EAIStatusMode::SeePlayer;
			// Chase Player
			MoveToActor(UGameplayStatics::GetPlayerPawn(this, 0));
			playerLastLocation = UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation();
			// Update Speed
			Cast<ABaseAI>(controlledAI)->SetMovementSpeed(status);
		}

		// No else to continue patrolling
	}

	// If AI is chasing player
	else if (status == EAIStatusMode::SeePlayer)
	{
		// If we can no longer see player, go to last position for player
		if (!canSeePlayer)
		{
			visibilityChecks = 0;
			status = EAIStatusMode::LookForPlayer;
			MoveToLocation(playerLastLocation);
		}
		// Otherwise continue chasing and update its location
		else
		{
			playerLastLocation = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
			MoveToActor(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		}
	}

	// If AI is looking for player old location
	else if (status == EAIStatusMode::LookForPlayer)
	{
		// if we can see player again, begin chasing player again
		if (canSeePlayer)
		{
			status = EAIStatusMode::SeePlayer;
			MoveToActor(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		}

		// Otherwise increment lockout and go back to patrol
		else
		{
			// Wait for duration to see player
			visibilityChecks++;
			if (visibilityChecks > maxChecks)
			{
				// Change Status
				status = EAIStatusMode::Patrolling;
				// Update Speed and go to next patrol point
				Cast<ABaseAI>(controlledAI)->SetMovementSpeed(status);
				GotoNextWaypoint();
			}

			// Rotate around
			else
			{
				controlledAI->AddActorWorldRotation(FRotator(0.0f, 7.2f, 0.0f), false);
			}
		}
	}
}
