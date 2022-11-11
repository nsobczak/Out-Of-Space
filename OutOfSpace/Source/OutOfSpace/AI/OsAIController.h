#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "OsAIController.generated.h"

class AOsCharacter;
/**
 * 
 */
UCLASS()
class OUTOFSPACE_API AOsAIController : public AAIController
{
	GENERATED_BODY()

	AOsAIController();

public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION(BlueprintPure, Category = "AI Controller")
	FORCEINLINE AOsCharacter* GetPossessedOsCharacter() const { return PossessedOsCharacter; };

protected:
	UPROPERTY(EditAnywhere, Category = "AI Controller")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, Category="AI Controller")
	bool bDebug = false;

	UPROPERTY(VisibleInstanceOnly, Category = "AI Controller", AdvancedDisplay, Transient)
	AOsCharacter* PossessedOsCharacter;
};
