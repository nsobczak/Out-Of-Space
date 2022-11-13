#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetBlackboardBool.generated.h"

/**
 * 
 */
UCLASS()
class OUTOFSPACE_API UBTTask_SetBlackboardBool : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override { Super::OnGameplayTaskActivated(Task); };
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override { Super::OnGameplayTaskDeactivated(Task); };

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	
protected:
	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector BlackboardKey_Bool;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	bool NewBoolValue = false;
	
private:
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (AllowPrivateAccess = "true"))
	bool DEBUG = false;
};
