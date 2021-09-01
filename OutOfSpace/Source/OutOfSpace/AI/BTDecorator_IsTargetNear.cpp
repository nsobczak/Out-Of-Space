#include "OutOfSpace/AI/BTDecorator_IsTargetNear.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "OutOfSpace/OutOfSpace.h"

void UBTDecorator_IsTargetNear::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		BlackboardKey_Target.ResolveSelectedKey(*BBAsset);
	}
}

bool UBTDecorator_IsTargetNear::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UE_LOG(LogOoS, Log, TEXT("UBTDecorator_IsTargetNear::CalculateRawConditionValue"));

	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
	if (!bb)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT( "failed to retrive UBlackboardComponent in UBTDecorator_IsTargetNear::CalculateRawConditionValue"
		       ));
		return false;
	}

	AActor* player = UGameplayStatics::GetPlayerCharacter(this, 0);
	bb->SetValueAsObject(BlackboardKey_Target.SelectedKeyName, player);

	return bb->GetValueAsObject(BlackboardKey_Target.SelectedKeyName)
		       ? (FVector::DistSquared(player->GetActorLocation(), OwnerComp.GetOwner()->GetActorLocation()) <
			       FMath::Square(DetectionDistance))
		       : false;
}
