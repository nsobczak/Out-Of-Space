#include "OutOfSpace/AI/BTTask_SetBlackboardBool.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "OutOfSpace/OutOfSpace.h"


void UBTTask_SetBlackboardBool::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		BlackboardKey_Bool.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UBTTask_SetBlackboardBool::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (DEBUG) { UE_LOG(LogOoS, Log, TEXT("set bb bool task")); }

	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
	if (!bb)
	{
		UE_LOG(LogOoS, Warning,
			TEXT( "failed to retrive UBlackboardComponent in UBTTask_SetBlackboardBool::ExecuteTask" ));
		return EBTNodeResult::Failed;
	}

	bb->SetValueAsBool(BlackboardKey_Bool.SelectedKeyName, NewBoolValue);

	return EBTNodeResult::Succeeded;
}
