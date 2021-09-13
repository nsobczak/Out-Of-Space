#include "W_DamageIndicatorElt.h"

#include "Kismet/KismetMathLibrary.h"

UW_DamageIndicatorElt::UW_DamageIndicatorElt(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	if (!IndicatorOpacityCurve)
	{
		static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("/Game/Curves/EaseInOut_C"));
		if (Curve.Succeeded())
		{
			IndicatorOpacityCurve = Curve.Object;
		}
	}
}

void UW_DamageIndicatorElt::NativeConstruct()
{
	Super::NativeConstruct();

	LifeTimeRemaining = LifeTime;
}

void UW_DamageIndicatorElt::UpdateOpacity()
{
	float alpha = (LifeTime - LifeTimeRemaining) / LifeTime;
	alpha = FMath::Clamp(alpha, 0.f, 1.f);
	float curveAlpha = alpha;

	if (IndicatorOpacityCurve != nullptr)
	{
		curveAlpha = IndicatorOpacityCurve->GetFloatValue(alpha);
	}

	IndicatorOpacity = FMath::Lerp(1.f, 0.f, curveAlpha);
}

void UW_DamageIndicatorElt::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	LifeTimeRemaining -= InDeltaTime;

	UpdateOpacity();

	FVector playerLoc = GetOwningPlayer()->GetViewTarget()->GetActorLocation();
	// We could destroy this if !DamageCauser
	DamageCauserLastLocation = DamageCauser ? DamageCauser->GetActorLocation() : DamageCauserLastLocation;
	FRotator worldRotator = UKismetMathLibrary::FindLookAtRotation(playerLoc, DamageCauserLastLocation);
	FRotator camRot = GetOwningPlayerCameraManager()->GetCameraRotation();
	SetRenderTransformAngle(worldRotator.Yaw - camRot.Yaw);

	if (LifeTimeRemaining <= 0)
	{
		RemoveFromViewport();
	}
}

void UW_DamageIndicatorElt::NativeDestruct()
{
	Super::NativeDestruct();
}
