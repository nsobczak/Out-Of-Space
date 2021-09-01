#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Fire.generated.h"

/**
 * 
 */
UCLASS()
class OUTOFSPACE_API UBTTask_Fire : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override { Super::OnGameplayTaskActivated(Task); };
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override { Super::OnGameplayTaskDeactivated(Task); };

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	
protected:
	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector BlackboardKey_Target;

private:
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (AllowPrivateAccess = "true"))
	bool DEBUG = false;
};
