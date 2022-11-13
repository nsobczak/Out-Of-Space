#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_StopMovement.generated.h"

/**
 * 
 */
UCLASS()
class OUTOFSPACE_API UBTTask_StopMovement : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override { Super::OnGameplayTaskActivated(Task); };
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override { Super::OnGameplayTaskDeactivated(Task); };

	// TODO: reset BlackboardKey_bIsMovingToTempLoc
// protected:
// 	UPROPERTY(EditAnywhere, Category=Blackboard)
// 	FBlackboardKeySelector BlackboardKey_bIsMovingToTempLoc;
	
private:
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (AllowPrivateAccess = "true"))
	bool DEBUG = false;
};
