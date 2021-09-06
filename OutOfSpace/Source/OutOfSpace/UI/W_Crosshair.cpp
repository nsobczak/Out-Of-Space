#include "OutOfSpace/UI/W_Crosshair.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"
#include "OutOfSpace/Character/OsPlayerController.h"


void UW_Crosshair::NativeConstruct()
{
	Super::NativeConstruct();

	OsPlayerController = Cast<AOsPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	UE_LOG(LogTemp, Warning, TEXT("test NativeConstruct"));
	if (OsPlayerController)
	{
		OsPlayerController->OnCrosshairLock.AddUniqueDynamic(this, &UW_Crosshair::Lock);
	}
}

void UW_Crosshair::NativeDestruct()
{
	Super::NativeDestruct();

	if (OsPlayerController)
	{
		OsPlayerController->OnCrosshairLock.RemoveDynamic(this, &UW_Crosshair::PlayLockAnimation);
	}
}

void UW_Crosshair::Lock(bool const bLock)
{
	if (bIsLocked != bLock)
	{
		bIsLocked = bLock;
		PlayLockAnimation(!bIsLocked);
	}
}

void UW_Crosshair::PlayLockAnimation(bool const bReverse)
{
	UUserWidget::PlayAnimation(LockAnimation, 0, 1,
		(bReverse ? EUMGSequencePlayMode::Reverse : EUMGSequencePlayMode::Forward));
}
