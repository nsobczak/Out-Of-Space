#include "OsGameMode.h"

#include "OsWorldSettings.h"
#include "Kismet/GameplayStatics.h"
#include "OutOfSpace/OutOfSpace.h"
#include "OutOfSpace/Character/OsPlayerCharacter.h"
#include "OutOfSpace/Character/OsPlayerController.h"
#include "OutOfSpace/UI/OsHUD.h"
#include "UObject/ConstructorHelpers.h"


AOsGameMode::AOsGameMode()
{
	PlayerControllerClass = AOsPlayerController::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Character/Player_BP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	else
	{
		DefaultPawnClass = AOsCharacter::StaticClass();
		UE_LOG(LogOoS, Error, TEXT("Failed to retrieve /Game/Character/Player_BP, using AOsCharacter::StaticClass()"));
	}

	HUDClass = AOsHUD::StaticClass();
}

void AOsGameMode::StartPlay()
{
	Super::StartPlay();

	bIsGamePlaying = true;
	OnIsGamePlayingUpdated.Broadcast(bIsGamePlaying);

	check(GEngine != nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("AOsGameMode StartPlay"));

	// bind player pawn
	OsPlayerController = Cast<AOsPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (OsPlayerController)
	{
		// TODO: test if event is called when changing possessed pawn
		OsPlayerController->OnNewPawnPossessed.AddUniqueDynamic(this, &AOsGameMode::HandleNewPlayerPawn);
		if (OsPlayerController->GetPawn())
		{
			HandleNewPlayerPawn(OsPlayerController->GetPawn());
		}
	}
	else
	{
		UE_LOG(LogOoS, Warning, TEXT("failed to retrieve OsPlayerController in AOsGameMode::StartPlay()"));
	}
}

void AOsGameMode::HandleNewPlayerPawn(APawn* pawn)
{
	UE_LOG(LogOoS, Log, TEXT("HandleNewPlayerPawn called for pawn %s"), *pawn->GetName());

	AOsCharacter* osCharacter = Cast<AOsCharacter>(pawn);
	OsCharacter = osCharacter;
	if (osCharacter)
	{
		osCharacter->OnFoeKilledCountUpdated.AddDynamic(this, &AOsGameMode::HandlePlayerFoeKilledCountUpdated);
		osCharacter->OnDeath.AddDynamic(this, &AOsGameMode::HandlePlayerDeath);
		// TODO: choose where to unbind old player pawn
	}
}

void AOsGameMode::HandlePlayerFoeKilledCountUpdated(int32 const newKilledCount)
{
	AOsWorldSettings* osWorldSettings = Cast<AOsWorldSettings>(GetWorldSettings());

	if (OsCharacter && osWorldSettings && OsCharacter->GetFoeKilledCount() >= osWorldSettings->TargetFoeAmount)
	{
		UE_LOG(LogOoS, Log, TEXT("Objective completed, player won"));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Objective completed, player won"));

		bIsGamePlaying = false;
		OnIsGamePlayingUpdated.Broadcast(bIsGamePlaying);

		GameResult = EGameResult::FGR_WON;
		OnGameOver.Broadcast(GameResult);
	}
}

void AOsGameMode::HandlePlayerDeath()
{
	if (OsCharacter && OsCharacter->IsDead())
	{
		UE_LOG(LogOoS, Log, TEXT("Player is dead, player lost"));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Player is dead, player lost"));

		bIsGamePlaying = false;
		OnIsGamePlayingUpdated.Broadcast(bIsGamePlaying);

		GameResult = EGameResult::FGR_LOST;
		OnGameOver.Broadcast(GameResult);
	}
}

void AOsGameMode::GameOverCheck()
{
	HandlePlayerFoeKilledCountUpdated(OsCharacter->GetFoeKilledCount());
	HandlePlayerDeath();
}
