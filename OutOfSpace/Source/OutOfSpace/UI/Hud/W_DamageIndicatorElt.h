#pragma once

#include "CoreMinimal.h"
#include "OutOfSpace/UI/OsWidget.h"
#include "W_DamageIndicatorElt.generated.h"

/**
 * 
 */
UCLASS()
class OUTOFSPACE_API UW_DamageIndicatorElt : public UOsWidget
{
	GENERATED_BODY()

public:
	UW_DamageIndicatorElt(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

	void SetDamageCauser(AActor*& newDamageCauser) { DamageCauser = newDamageCauser; };

	void SetLifeTime(float newTime) { LifeTime = newTime; };

	UFUNCTION(BlueprintPure, Category="DamageIndicator")
	float GetIndicatorOpacity() const { return IndicatorOpacity; };

protected:
	AActor* DamageCauser;

	// Time the widget will be displayed on screen
	UPROPERTY(BlueprintReadOnly, Category="DamageIndicator")
	float LifeTime = 3.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="DamageIndicator", Transient)
	float LifeTimeRemaining;

	float IndicatorOpacity = 1.f;

	// Curve for IndicatorOpacity
	UPROPERTY(EditAnywhere, Category = "DamageIndicator")
	UCurveFloat* IndicatorOpacityCurve = nullptr;

	void UpdateOpacity();

private:
	UPROPERTY(Transient)
	FVector DamageCauserLastLocation;
};
