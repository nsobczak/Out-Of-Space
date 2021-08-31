#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "OsWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class OUTOFSPACE_API AOsWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Goal")
	int32 TargetFoeAmount = 1;
};
