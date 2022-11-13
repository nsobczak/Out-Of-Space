#include "OutOfSpace/AI/BTTask_StopMovement.h"

#include "OsAIController.h"
#include "OutOfSpace/OutOfSpace.h"


EBTNodeResult::Type UBTTask_StopMovement::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (DEBUG) { UE_LOG(LogOoS, Log, TEXT("UBTTask_StopMovement")); }

	AOsAIController* osAiController = Cast<AOsAIController>(OwnerComp.GetAIOwner());
	if (!osAiController)
	{
		UE_LOG(LogOoS, Warning, TEXT("osAiController is null in UBTTask_StopMovement::ExecuteTask"));
		return EBTNodeResult::Failed;
	}

	osAiController->StopMovement();
	return EBTNodeResult::Succeeded;
}
