#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProgressBarComponent.generated.h"


UCLASS(Abstract, ClassGroup=(ProgressBar), meta=(BlueprintSpawnableComponent))
class OUTOFSPACE_API UProgressBarComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UProgressBarComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, Category = "ProgressBar")
	float MaxValue = 100.f;

	// Get current value and divide by max
	UFUNCTION(BlueprintPure, Category = "ProgressBar")
	FORCEINLINE float GetTargetRatio() const { return TargetValue / MaxValue; };

	// Get current value and divide by max
	UFUNCTION(BlueprintPure, Category = "ProgressBar")
	FORCEINLINE float GetCurrentRatio() const { return CurrentValue / MaxValue; };

	UFUNCTION(BlueprintCallable, Category="ProgressBar")
	FORCEINLINE void ResetValue() { TargetValue = MaxValue; };

	UFUNCTION(BlueprintCallable, Category="ProgressBar")
	virtual void Add(float amount);

	UFUNCTION(BlueprintCallable, Category="ProgressBar")
	virtual void Empty() { TargetValue = 0; };

	// Check TargetValue
	UFUNCTION(BlueprintPure, Category="ProgressBar")
	FORCEINLINE bool IsEmpty() const { return TargetValue <= 0; };

protected:
	virtual void BeginPlay() override;

	// Set to MaxValue on begin play
	UPROPERTY(VisibleInstanceOnly, Category = "ProgressBar|Debug")
	float CurrentValue = 1.f;

	// Set to MaxValue on begin play
	UPROPERTY(VisibleInstanceOnly, Category = "ProgressBar|Debug")
	float TargetValue = 1.f;

	// Time to remaining for current value to reach target value
	float TimeRemainingToReachTarget;

	// Time CurrentValue will spend updating to reach TargetValue
	UPROPERTY(EditDefaultsOnly, Category = "ProgressBar|Animation")
	float TimeToReachTarget = 1.5f;

	// Time before CurrentValue starts updating to TargetValue
	UPROPERTY(EditDefaultsOnly, Category = "ProgressBar|Animation")
	float DelayStartUpdateToTarget = 0.7f;

	UFUNCTION()
	void SetTimeRemainingToReachTarget(float time) { TimeRemainingToReachTarget = time; };

	UFUNCTION()
	void UpdateCurrentVal(float DeltaTime);

	// Curve for current value to reach target value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProgressBar|Animation")
	UCurveFloat* CurrentValueCurve = nullptr;

private:
	// Set to MaxValue on begin play
	UPROPERTY(VisibleInstanceOnly, Category = "ProgressBar|Debug")
	float LastValue = 1.f;
};
