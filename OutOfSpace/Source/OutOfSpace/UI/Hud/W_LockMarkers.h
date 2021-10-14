#pragma once

#include "CoreMinimal.h"
#include "OutOfSpace/UI/OsWidget.h"
#include "W_LockMarkers.generated.h"

USTRUCT()
struct FLockMarker
{
	GENERATED_BODY()

	UOsWidget* Marker;

	// For lerp
	FVector2D StartingLerpRelativeLocation = FVector2D::ZeroVector;
	// For lerp	
	FVector2D TargetLerpRelativeLocation = FVector2D::ZeroVector;

	// Location when a target is detected
	FVector2D LockedLocation = FVector2D::ZeroVector;
	// Location when no target is detected
	FVector2D UnlockedLocation = FVector2D::ZeroVector;

	FLockMarker(): Marker(nullptr)
	{
	};

	FLockMarker(UOsWidget* markerWidget, FVector2D nLockedLocation, FVector2D nUnlockedLerpLocation):
		Marker(markerWidget), LockedLocation(nLockedLocation), UnlockedLocation(nUnlockedLerpLocation)
	{
		if (markerWidget)
		{
			StartingLerpRelativeLocation = markerWidget->RenderTransform.Translation;
			TargetLerpRelativeLocation = markerWidget->RenderTransform.Translation;
		}
	};
};

/**
 * 
 */
UCLASS()
class OUTOFSPACE_API UW_LockMarkers : public UOsWidget
{
	GENERATED_BODY()

public:
	UW_LockMarkers(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

	void DestroyMarkers();
	void UpdateMarkerCount();

	UFUNCTION(BlueprintCallable, Category="LockMarkers")
	void Lock(bool const bLock = true);

protected:
	class AOsPlayerController* OsPlayerController;

	void PlayLockAnimation(bool const bReverse = false);

	UPROPERTY(EditAnywhere, Category="LockMarkers")
	TSubclassOf<UUserWidget> MarkerWidgetClass;

	UPROPERTY(EditAnywhere, Category="LockMarkers")
	float RadiusWhenLocked = 50.f;
	UPROPERTY(EditAnywhere, Category="LockMarkers")
	float RadiusWhenUnlocked = 200.f;

	// Curve for current value to reach target value
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LockMarkers|Animation")
	UCurveFloat* LerpAnimationCurve = nullptr;

private:
	UPROPERTY(VisibleInstanceOnly, Category="LockMarkers|Debug")
	TArray<FLockMarker> Markers;

	UPROPERTY(VisibleInstanceOnly, Category="LockMarkers|Debug")
	bool bIsLocked = false;

	UPROPERTY(EditAnywhere, Category="LockMarkers|Animation")
	float TimeToLerp = 0.4f;
	float TimeRemainingLerping;
};
