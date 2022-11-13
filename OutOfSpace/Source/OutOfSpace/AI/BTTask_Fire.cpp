#include "OutOfSpace/AI/BTTask_Fire.h"

#include "OsAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "OutOfSpace/OutOfSpace.h"
#include "OutOfSpace/Character/OsCharacter.h"


void UBTTask_Fire::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		BlackboardKey_Target.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UBTTask_Fire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (DEBUG) { UE_LOG(LogOoS, Log, TEXT("fire task")); }

	AOsAIController* osAiController = Cast<AOsAIController>(OwnerComp.GetAIOwner());
	if (!osAiController)
	{
		UE_LOG(LogOoS, Warning, TEXT("osAiController is null in UBTTask_Fire::ExecuteTask"));
		return EBTNodeResult::Failed;
	}

	AOsCharacter* possessedOsChara = osAiController->GetPossessedOsCharacter();
	if (!possessedOsChara)
	{
		UE_LOG(LogOoS, Warning, TEXT("osChara is null in UBTTask_Fire::ExecuteTask"));
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
	if (!bb)
	{
		UE_LOG(LogOoS, Warning, TEXT( "failed to retrive UBlackboardComponent in UBTTask_Fire::ExecuteTask" ));
		return EBTNodeResult::Failed;
	}


	AActor* targetObject = Cast<AActor>(bb->GetValueAsObject(BlackboardKey_Target.SelectedKeyName));
	if (!targetObject)
	{
		UE_LOG(LogOoS, Error, TEXT("targetObject is null"));
		return EBTNodeResult::Failed;
	}
	
	// Rotate towards targetLoc
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(possessedOsChara->GetActorLocation(),
	                                                      targetObject->GetActorLocation());
	possessedOsChara->SetActorRotation(rot, ETeleportType::TeleportPhysics);
	// possessedOsChara->SetActorLocation(FVector(0, 0, 0));

	possessedOsChara->Fire(EFireType::FT_SIMPLE);

	return EBTNodeResult::Succeeded;
}
