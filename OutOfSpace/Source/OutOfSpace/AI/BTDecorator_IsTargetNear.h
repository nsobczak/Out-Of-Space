#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsTargetNear.generated.h"

/**
 * 
 */
UCLASS()
class OUTOFSPACE_API UBTDecorator_IsTargetNear : public UBTDecorator
{
	GENERATED_BODY()

	UBTDecorator_IsTargetNear()
	{
		NodeName = "Check if target is near";
	}

public:
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override { Super::OnGameplayTaskActivated(Task); };
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override { Super::OnGameplayTaskDeactivated(Task); };

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	// virtual FString GetStaticDescription() const override;

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	//
	// #if WITH_EDITOR
	// 	virtual FName GetNodeIconName() const override;
	// #endif // WITH_EDITOR


protected:
	/** blackboard key selector */
	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector BlackboardKey_Target;

	// TODO: move DetectionDistance into character or controller
	UPROPERTY(EditAnywhere, Category=Movement)
	float DetectionDistance = 7500.f;
};
