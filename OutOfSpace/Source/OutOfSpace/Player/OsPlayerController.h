#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OsPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class OUTOFSPACE_API AOsPlayerController : public APlayerController
{
	GENERATED_BODY()

	// TODO: add default constructor
public:
	AOsPlayerController(const FObjectInitializer& ObjInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
