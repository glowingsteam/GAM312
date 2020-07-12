// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GAM312GameMode.h"
#include "GAM312HUD.h"
#include "GAM312Character.h"
#include "UObject/ConstructorHelpers.h"

AGAM312GameMode::AGAM312GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AGAM312HUD::StaticClass();
}
