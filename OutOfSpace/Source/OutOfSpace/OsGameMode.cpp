// Copyright Epic Games, Inc. All Rights Reserved.

#include "OsGameMode.h"
#include "OutOfSpace/Player/OsPlayerCharacter.h"
#include "Player/OsPlayerController.h"
#include "UI/OsHUD.h"
#include "UObject/ConstructorHelpers.h"

AOsGameMode::AOsGameMode()
{
	PlayerControllerClass = AOsPlayerController::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Player/Player_BP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	else
	{
		DefaultPawnClass = AOsCharacter::StaticClass();
	}

	HUDClass = AOsHUD::StaticClass();
	// GameStateClass = AGameState::StaticClass();
}

void AOsGameMode::StartPlay()
{
	Super::StartPlay();

	check(GEngine != nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("AOsGameMode StartPlay"));
}
