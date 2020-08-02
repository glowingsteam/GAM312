// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "AIStatusMode.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EAIStatusMode : uint8
{
	Patrolling		UMETA(DisplayName = "Patrolling"),
	SeePlayer		UMETA(DisplayName = "See Player"),
	LookForPlayer	UMETA(DisplayName = "Look For Player")
};

UCLASS()
class GAM312_API UAIStatusMode : public UUserDefinedEnum
{
	GENERATED_BODY()
	
};
