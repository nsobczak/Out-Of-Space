#include "OutOfSpace/AI/BTTask_FlyTo.h"

#include "OsAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "OutOfSpace/OutOfSpace.h"
#include "OutOfSpace/Character/OsCharacter.h"


void UBTTask_FlyTo::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		BlackboardKey_Target.ResolveSelectedKey(*BBAsset);
		BlackboardKey_bIsMovingToTempLoc.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UBTTask_FlyTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (DEBUG) { UE_LOG(LogOoS, Log, TEXT("moveTo task")); }

	AOsAIController* osAiController = Cast<AOsAIController>(OwnerComp.GetAIOwner());
	if (!osAiController)
	{
		UE_LOG(LogOoS, Warning, TEXT("osAiController is null in UBTTask_FlyTo::ExecuteTask"));
		return EBTNodeResult::Failed;
	}

	AOsCharacter* possessedOsChara = osAiController->GetPossessedOsCharacter();
	if (!possessedOsChara)
	{
		UE_LOG(LogOoS, Warning, TEXT("osChara is null in UBTTask_FlyTo::ExecuteTask"));
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
	if (!bb)
	{
		UE_LOG(LogOoS, Warning, TEXT( "failed to retrive UBlackboardComponent in UBTTask_FlyTo::ExecuteTask" ));
		return EBTNodeResult::Failed;
	}


	AActor* targetObject = Cast<AActor>(bb->GetValueAsObject(BlackboardKey_Target.SelectedKeyName));
	if (!targetObject)
	{
		UE_LOG(LogOoS, Error, TEXT("targetObject is null"));
		return EBTNodeResult::Failed;
	}


	FVector traceStartLoc = possessedOsChara->GetActorLocation();
	FVector targetLoc = targetObject->GetActorLocation();
	TArray<AActor*> actorsToIgnore = {possessedOsChara};
	FHitResult hitResult;
	bool hitTarget = false;
	const int32 maxTry = 8.f;

	// 1 trace
	hitTarget = UKismetSystemLibrary::SphereTraceSingle(possessedOsChara->GetWorld(),
		traceStartLoc, targetLoc, SphereTraceRadius, ETraceTypeQuery::TraceTypeQuery1, false,
		actorsToIgnore, EDrawDebugTrace::ForDuration, hitResult, true, FColor::Red, FColor::Green, 2.5f);

	if (!hitTarget || hitResult.GetActor() == targetObject)
	{
		// Rotate towards targetLoc
		FRotator rot = UKismetMathLibrary::FindLookAtRotation(possessedOsChara->GetActorLocation(), targetLoc);
		possessedOsChara->SetActorRotation(rot, ETeleportType::TeleportPhysics);

		osAiController->MoveToLocation(targetLoc, AcceptanceRadius, true, false, false, true);

		return EBTNodeResult::Succeeded;
	}

	const FVector obstacleLocation = hitResult.Location;

	// 2 traces
	for (int currentTry = 0; currentTry < maxTry; ++currentTry)
	{
		UE_LOG(LogOoS, Log, TEXT("UBTTask_FlyTo - in else, currentTry = %d"), currentTry);

		traceStartLoc = possessedOsChara->GetActorLocation();

		const float variationDist = 500.f * (currentTry > 3 ? 2.5f : 1.f);
		FVector directionVector = currentTry == 0 ? possessedOsChara->GetActorUpVector() :
			currentTry == 1 ? possessedOsChara->GetActorRightVector() :
			currentTry == 2 ? -possessedOsChara->GetActorUpVector() :
			currentTry == 3 ? -possessedOsChara->GetActorRightVector() :
			currentTry == 4 ? possessedOsChara->GetActorUpVector() :
			currentTry == 5 ? possessedOsChara->GetActorRightVector() :
			currentTry == 6 ? -possessedOsChara->GetActorUpVector() :
			-possessedOsChara->GetActorRightVector();
		targetLoc = obstacleLocation + variationDist * directionVector;

		hitTarget = UKismetSystemLibrary::SphereTraceSingle(possessedOsChara->GetWorld(),
			traceStartLoc, targetLoc, SphereTraceRadius, ETraceTypeQuery::TraceTypeQuery1, false,
			actorsToIgnore, EDrawDebugTrace::ForDuration, hitResult, true, FColor::Orange, FColor::Blue, 2.5f);

		if (!hitTarget)
		{
			traceStartLoc = targetLoc;
			targetLoc = targetObject->GetActorLocation();


			hitTarget = UKismetSystemLibrary::SphereTraceSingle(possessedOsChara->GetWorld(),
				traceStartLoc, targetLoc, SphereTraceRadius, ETraceTypeQuery::TraceTypeQuery1, false,
				actorsToIgnore, EDrawDebugTrace::ForDuration, hitResult, true, FColor::Red, FColor::Green, 2.5f);

			if (!hitTarget || hitResult.GetActor() == targetObject)
			{
				// Rotate towards targetLoc
				FRotator rot = UKismetMathLibrary::FindLookAtRotation(possessedOsChara->GetActorLocation(), traceStartLoc);
				possessedOsChara->SetActorRotation(rot, ETeleportType::TeleportPhysics);

				osAiController->MoveToLocation(traceStartLoc, AcceptanceRadius, true, false, false, true);

				bb->SetValueAsBool(BlackboardKey_bIsMovingToTempLoc.SelectedKeyName, true);
				
				return EBTNodeResult::Succeeded;
			}
			else
			{
				traceStartLoc = possessedOsChara->GetActorLocation();
				targetLoc = targetObject->GetActorLocation();
			}
		}
		else
		{
			traceStartLoc = possessedOsChara->GetActorLocation();
			targetLoc = targetObject->GetActorLocation();
		}
	}

	if (DEBUG) { UE_LOG(LogOoS, Warning, TEXT("moveTo task - failed to find a valid path")); }
	return EBTNodeResult::Failed;
}
