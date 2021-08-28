#include "OutOfSpace/Player/OsPlayerController.h"

AOsPlayerController::AOsPlayerController(const FObjectInitializer& ObjInitializer) : Super(
	ObjInitializer)
{
}

void AOsPlayerController::BeginPlay()
{
	Super::BeginPlay();


	// WorldSettings = 
	// GameInstance 
}

void AOsPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AOsPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
