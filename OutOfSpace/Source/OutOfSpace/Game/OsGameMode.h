#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OsGameMode.generated.h"

UENUM(BlueprintType)
enum class EGameResult : uint8
{
	FGR_WON UMETA(DisplayName = "Player won"),
	FGR_LOST UMETA(DisplayName = "Player lost"),
	FGR_OTHER UMETA(DisplayName = "Other"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameOverEvent, EGameResult const, result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBoolEvent, bool const, value);

UCLASS(minimalapi)
class AOsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AOsGameMode();

	virtual void StartPlay() override;

	UFUNCTION(BlueprintCallable, Category="Game")
	void GameOverCheck();

	UPROPERTY(VisibleAnywhere, Category="Game")
	bool bIsGamePlaying = false;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Game")
	FBoolEvent OnIsGamePlayingUpdated;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Game")
	FGameOverEvent OnGameOver;

	UFUNCTION(BlueprintPure, Category="Game")
	FORCEINLINE EGameResult GetGameResult() const { return GameResult; };

protected:
	UPROPERTY(Transient)
	class AOsPlayerController* OsPlayerController;
	UPROPERTY(Transient)
	class AOsCharacter* OsCharacter;

	UFUNCTION()
	void HandleNewPlayerPawn(APawn* pawn);

	UFUNCTION()
	void HandlePlayerFoeKilledCountUpdated(int32 const newKilledCount);
	UFUNCTION()
	void HandlePlayerDeath();

	UPROPERTY(VisibleAnywhere, Category="Game")
	EGameResult GameResult = EGameResult::FGR_OTHER;
};
