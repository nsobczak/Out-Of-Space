// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OsGameMode.generated.h"

UCLASS(minimalapi)
class AOsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AOsGameMode();
	
	virtual  void StartPlay() override;
};



