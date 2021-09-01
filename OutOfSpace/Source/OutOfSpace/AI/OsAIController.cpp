#include "OutOfSpace/AI/OsAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "OutOfSpace/OutOfSpace.h"
#include "OutOfSpace/Character/OsCharacter.h"

AOsAIController::AOsAIController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> bt(TEXT("/Game/Character/Foe_BT"));
	if (bt.Succeeded())
	{
		BehaviorTree = bt.Object;
	}
};

void AOsAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (bDebug)
	{
		UE_LOG(LogOoS, Log, TEXT("OnPossess called for pawn %s"),
		       InPawn?*InPawn->GetName(): TEXT("null"));
	};

	PossessedOsCharacter = Cast<AOsCharacter>(InPawn);
	if (!PossessedOsCharacter)
	{
		UE_LOG(LogOoS, Error, TEXT("PossessedOsCharacter is null in AOsAIController::OnPossess"));
		return;
	}

	if (!BehaviorTree)
	{
		UE_LOG(LogOoS, Warning, TEXT("CurrentBehaviorTree is null in ANPCAIController::OnPossess"));
	}

	FTimerHandle timerHandle;
	FTimerDelegate timerDelegate;
	// timerDelegate.BindUObject(this, &AOsAIController::RunBehaviorTree, BehaviorTree);
	timerDelegate.BindUFunction(this, FName("RunBehaviorTree"), BehaviorTree);
	GetWorldTimerManager().SetTimer(timerHandle, timerDelegate, 1.5f + FMath::RandRange(-0.5f, 0.5f), false);
	// RunBehaviorTree(BehaviorTree);

	if (bDebug)
	{
		UE_LOG(LogOoS, Log, TEXT("%s possessing %s is running bt %s"),
		       *GetName(), *PossessedOsCharacter->GetName(), *BehaviorTree->GetName());
	};
}

void AOsAIController::OnUnPossess()
{
	Super::OnUnPossess();
}
