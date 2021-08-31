#include "OutOfSpace/UI/OsHUD.h"

#include "OsWidget.h"
#include "Engine/Canvas.h"
#include "OutOfSpace/OutOfSpace.h"
#include "OutOfSpace/Character/OsPlayerController.h"


AOsHUD::AOsHUD()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bTickEvenWhenPaused = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> hudClass(TEXT("/Game/UI/W_hud"));
	if (hudClass.Class != NULL)
	{
		HUDWidgetClass = hudClass.Class;
	}
}

// void AOsHUD::DrawHUD()
// {
// 	Super::DrawHUD();
//
// }


void AOsHUD::BeginPlay()
{
	Super::BeginPlay();

	//hud
	PlayerController = this->GetOwningPlayerController();
	if (WidgetClassToShow != EHudWidget::FHW_NONE)
	{
		switch (WidgetClassToShow)
		{
		case EHudWidget::FHW_START:
			ShowStartingWidget();
			break;
		case EHudWidget::FHW_HUD:
			ShowHUDWidget();
			break;
		case EHudWidget::FHW_PAUSE:
			ShowPauseWidget();
			break;
		case EHudWidget::FHW_END:
			ShowEndingWidget();
			break;
		default:
			break;
		}
	}
}


#pragma region Widgets
//==============================================================================================
void AOsHUD::ShowCursor(bool showCursor)
{
	if (PlayerController)
	{
		//UE_LOG(LogTemp, Warning, TEXT("before if - showCursor = %s"), showCursor ? TEXT("true") : TEXT("false"));
		PlayerController->bShowMouseCursor = showCursor;
		PlayerController->bEnableClickEvents = showCursor;
		PlayerController->bEnableMouseOverEvents = showCursor;
	}
}

UUserWidget* AOsHUD::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass, bool showCursor)
{
	if (CurrentWidget != nullptr)
	{
		OldWidgetClass = CurrentWidget->GetClass();
		CurrentWidget->RemoveFromViewport();
		CurrentWidget->RemoveFromParent();
		CurrentWidget->Destruct();
		CurrentWidget = nullptr;
	}

	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
			CurrentWidgetClass = NewWidgetClass;
		}

		ShowCursor(showCursor);
	}

	return CurrentWidget;
}

bool AOsHUD::IsShowingWidget(EHudWidget widgetToLookFor)
{
	switch (widgetToLookFor)
	{
	case EHudWidget::FHW_START:
		return CurrentWidgetClass == StartingWidgetClass;
		break;

	case EHudWidget::FHW_HUD:
		return CurrentWidgetClass == HUDWidgetClass;
		break;

	case EHudWidget::FHW_PAUSE:
		return CurrentWidgetClass == PauseWidgetClass;
		break;

	case EHudWidget::FHW_END:
		return CurrentWidgetClass == EndingWidgetClass;
		break;

	default:
		return false;
		break;
	}
}

void AOsHUD::ShowWidget(TSubclassOf<UUserWidget> NewWidgetClass, bool showCursor)
{
	if (NewWidgetClass != nullptr)
	{
		//OldWidgetClass = NewWidgetClass;
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}

		ShowCursor(showCursor);
	}
}

void AOsHUD::ShowStartingWidget()
{
	ChangeMenuWidget(StartingWidgetClass, false);
}

void AOsHUD::ShowEndingWidget()
{
	ChangeMenuWidget(EndingWidgetClass, false);
}

void AOsHUD::ShowPauseWidget()
{
	ChangeMenuWidget(PauseWidgetClass, false);
}

void AOsHUD::ShowHUDWidget()
{
	ChangeMenuWidget(HUDWidgetClass, false);
}


void AOsHUD::HideWidget(UUserWidget* widgetToHide, bool showCursor)
{
	if (widgetToHide != nullptr)
	{
		widgetToHide->RemoveFromViewport();
		widgetToHide->RemoveFromParent();
		widgetToHide->Destruct();
	}

	ShowCursor(showCursor);
}

void AOsHUD::HideCurrentWidget(bool showCursor)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget->RemoveFromParent();
		CurrentWidget->Destruct();
		CurrentWidget = nullptr;
	}

	ShowCursor(showCursor);
}

//==============================================================================================
#pragma endregion
