#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "Kismet/GameplayStatics.h"
#include "OutOfSpace/Character/OsCharacter.h"
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

	// Retrieve number of foes in the world and set TargetFoeAmount with it
	UFUNCTION(CallInEditor, Category = "Goal")
	void SetTargetFoeAmountToMax()
	{
		TargetFoeAmount = 0;

		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(this, AOsCharacter::StaticClass(), actors);
		TargetFoeAmount = actors.Num();
	}
};
