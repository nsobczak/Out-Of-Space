#include "W_LockMarkers.h"

#include "Components/PanelWidget.h"
#include "Kismet/GameplayStatics.h"
#include "OutOfSpace/Character/OsPlayerCharacter.h"
#include "OutOfSpace/Character/OsPlayerController.h"
#include "OutOfSpace/Component/LockControllerComponent.h"
#include "UObject/ConstructorHelpers.h"


UW_LockMarkers::UW_LockMarkers(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	if (!MarkerWidgetClass)
	{
		static ConstructorHelpers::FClassFinder<UUserWidget>
			markerClass(TEXT("/Game/UI/Hud/Crosshair/W_LockMarker_BP"));
		if (markerClass.Succeeded())
		{
			MarkerWidgetClass = markerClass.Class;
		}
	}

	if (!LerpAnimationCurve)
	{
		static ConstructorHelpers::FObjectFinder<UCurveFloat> curve(TEXT("/Game/Curves/EaseInOut_C"));
		if (curve.Succeeded())
		{
			LerpAnimationCurve = curve.Object;
		}
	}
}

void UW_LockMarkers::NativeConstruct()
{
	Super::NativeConstruct();

	OsPlayerController = Cast<AOsPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (OsPlayerController)
	{
		OsPlayerController->OnCrosshairLock.AddUniqueDynamic(this, &UW_LockMarkers::Lock);
	}

	OsPlayer = Cast<AOsPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (OsPlayer)
	{
		LockControllerComp = Cast<ULockControllerComponent>(
			OsPlayer->GetComponentByClass(ULockControllerComponent::StaticClass()));
		if (LockControllerComp)
		{
			LockControllerComp->OnLockedCharactersUpdated.AddUniqueDynamic(this, &UW_LockMarkers::UpdateDisplay);
		}
	}

	UpdateMarkerCount();
}

void UW_LockMarkers::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	// if time remaining lerping > 0 -> do update positions
	if (TimeRemainingLerping > 0 && Markers.Num() > 0)
	{
		TimeRemainingLerping -= InDeltaTime;

		float alpha = (TimeToLerp - TimeRemainingLerping) / TimeToLerp;
		alpha = FMath::Clamp(alpha, 0.f, 1.f);
		float curveAlpha = alpha;

		if (LerpAnimationCurve != nullptr)
		{
			curveAlpha = LerpAnimationCurve->GetFloatValue(alpha);
		}

		for (int i = 0; i < Markers.Num(); ++i)
		{
			if (Markers[i].Marker)
			{
				FVector2D newLocation = FMath::Lerp(Markers[i].StartingLerpRelativeLocation,
					Markers[i].TargetLerpRelativeLocation,
					curveAlpha);

				Markers[i].Marker->SetRenderTranslation(newLocation);

				// if (TimeRemainingLerping <= 0)
				// {
				// 	LastValue = TargetValue;
				// }
			}
		}
	}
}


void UW_LockMarkers::NativeDestruct()
{
	Super::NativeDestruct();

	DestroyMarkers();

	if (OsPlayerController)
	{
		OsPlayerController->OnCrosshairLock.RemoveDynamic(this, &UW_LockMarkers::Lock);
	}

	if (LockControllerComp)
	{
		LockControllerComp->OnLockedCharactersUpdated.RemoveDynamic(this, &UW_LockMarkers::UpdateDisplay);
	}
}

void UW_LockMarkers::DestroyMarkers()
{
	for (int i = 0; i < Markers.Num(); ++i)
	{
		if (Markers[i].Marker)
		{
			Markers[i].Marker->RemoveFromViewport();
			Markers[i].Marker->RemoveFromParent();
			Markers[i].Marker->Destruct();
			Markers[i].Marker = nullptr;
		}
	}
	Markers.Reset();
}

void UW_LockMarkers::UpdateMarkerCount()
{
	if (OsPlayer)
	{
		ULockControllerComponent* lockController = Cast<ULockControllerComponent>(
			OsPlayer->GetComponentByClass(ULockControllerComponent::StaticClass()));
		if (lockController)
		{
			DestroyMarkers();
			for (int i = 0; i < lockController->GetLockMaxCount(); ++i)
			{
				// create widget and add to Markers
				if (MarkerWidgetClass)
				{
					UOsWidget* markerWidget = CreateWidget<UOsWidget>(GetWorld(), MarkerWidgetClass);
					if (markerWidget)
					{
						double angle = i * 2.f * PI / (float)lockController->GetLockMaxCount() +
							(lockController->GetLockMaxCount() % 2 != 0 ? -PI / 2.f : 0);
						FVector2D lockedLocation = FVector2D(RadiusWhenLocked * cos(angle),
							RadiusWhenLocked * sin(angle));
						FVector2D unlockedLerpLocation = FVector2D(RadiusWhenUnlocked * cos(angle),
							RadiusWhenUnlocked * sin(angle));

						markerWidget->SetRenderTranslation(unlockedLerpLocation);

						Markers.Add(FLockMarker(markerWidget, lockedLocation, unlockedLerpLocation));
						markerWidget->AddToViewport();
					}
				}
			}
		}
	}
}

void UW_LockMarkers::Lock(bool const bLock)
{
	if (bIsLocked != bLock)
	{
		bIsLocked = bLock;
		PlayLockAnimation(!bIsLocked);
	}
}

void UW_LockMarkers::PlayLockAnimation(bool const bReverse)
{
	// TODO: handle case where we cancel animation by reversing it

	if (!bIsLocked && bReverse)
	{
		// to exterior
		for (int i = 0; i < Markers.Num(); ++i)
		{
			Markers[i].StartingLerpRelativeLocation = Markers[i].LockedLocation;
			Markers[i].TargetLerpRelativeLocation = Markers[i].UnlockedLocation;
		}
	}
	else if (bIsLocked && !bReverse)
	{
		// to center
		for (int i = 0; i < Markers.Num(); ++i)
		{
			Markers[i].StartingLerpRelativeLocation = Markers[i].UnlockedLocation;
			Markers[i].TargetLerpRelativeLocation = Markers[i].LockedLocation;
		}
	}

	TimeRemainingLerping = TimeToLerp;
}

void UW_LockMarkers::UpdateDisplay()
{
	if (LockControllerComp)
	{
		int32 visibleMarkerCount = LockControllerComp->GetLockMaxCount() - LockControllerComp->
			GetLockedCharacterCount();

		for (int i = 0; i < Markers.Num(); ++i)
		{
			if (Markers[i].Marker)
			{
				bool bIsVisible = i < LockControllerComp->GetLockMaxCount() - LockControllerComp->
					GetLockedCharacterCount();
				Markers[i].Marker->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			}
		}
	}
}
