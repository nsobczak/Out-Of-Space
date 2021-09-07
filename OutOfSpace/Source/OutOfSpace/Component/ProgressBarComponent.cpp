#include "ProgressBarComponent.h"

#include "OutOfSpace/OutOfSpace.h"

UProgressBarComponent::UProgressBarComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bTickEvenWhenPaused = false;

	if (!CurrentValueCurve)
	{
		static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("/Game/Curves/EaseInOut_C"));
		if (Curve.Succeeded())
		{
			CurrentValueCurve = Curve.Object;
		}
	}
}


void UProgressBarComponent::BeginPlay()
{
	Super::BeginPlay();

	TargetValue = MaxValue;
	CurrentValue = TargetValue;
	LastValue = TargetValue;
}

void UProgressBarComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TimeRemainingToReachTarget > 0)
	{
		UpdateCurrentVal(DeltaTime);
	}
}

void UProgressBarComponent::Add(float amount)
{
	TargetValue += amount;
	TargetValue = FMath::Clamp(TargetValue, 0.f, MaxValue);

	if (CurrentValue != TargetValue)
	{
		if (TimeRemainingToReachTarget > 0)
		{
			TimeRemainingToReachTarget = 0; // Stop updating
			LastValue = CurrentValue;
		}

		FTimerHandle timerHandle;
		FTimerDelegate timerDelegate;
		timerDelegate.BindUObject(this, &UProgressBarComponent::SetTimeRemainingToReachTarget, TimeToReachTarget);
		GetWorld()->GetTimerManager().SetTimer(timerHandle, timerDelegate, 0.2f, false, DelayStartUpdateToTarget);
	}
}

void UProgressBarComponent::UpdateCurrentVal(float DeltaTime)
{
	TimeRemainingToReachTarget -= DeltaTime;

	float alpha = (TimeToReachTarget - TimeRemainingToReachTarget) / TimeToReachTarget;
	alpha = FMath::Clamp(alpha, 0.f, 1.f);
	float curveAlpha = alpha;

	if (CurrentValueCurve != nullptr)
	{
		curveAlpha = CurrentValueCurve->GetFloatValue(alpha);
	}

	CurrentValue = FMath::Lerp(LastValue, TargetValue, curveAlpha);

	if (TimeRemainingToReachTarget <= 0)
	{
		LastValue = TargetValue;
	}
}
