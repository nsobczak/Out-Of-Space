// Copyright Epic Games, Inc. All Rights Reserved.

#include "OutOfSpaceGameMode.h"
#include "OutOfSpace/Player/OsPlayerCharacter.h"
#include "Player/OsPlayerController.h"
#include "UObject/ConstructorHelpers.h"

AOutOfSpaceGameMode::AOutOfSpaceGameMode()
{
	PlayerControllerClass = AOsPlayerController::StaticClass();
	
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Player/Player_BP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	else
	{
		DefaultPawnClass = AOutOfSpaceCharacter::StaticClass();
	}

	
	// HUDClass = AHUD::StaticClass();
	// GameStateClass = AGameState::StaticClass();
}
