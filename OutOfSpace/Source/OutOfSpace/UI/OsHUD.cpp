#include "OutOfSpace/UI/OsHUD.h"

#include "OsWidget.h"
#include "Engine/Canvas.h"
#include "Kismet/GameplayStatics.h"
#include "OutOfSpace/OutOfSpace.h"
#include "OutOfSpace/Character/OsPlayerController.h"
#include "OutOfSpace/Game/OsGameMode.h"


AOsHUD::AOsHUD()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bTickEvenWhenPaused = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> hudClass(TEXT("/Game/UI/Hud/W_hud"));
	if (hudClass.Class != NULL)
	{
		HUDWidgetClass = hudClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> endClass(TEXT("/Game/UI/W_result"));
	if (endClass.Class != NULL)
	{
		EndingWidgetClass = endClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> pauseClass(TEXT("/Game/UI/W_Pause"));
	if (pauseClass.Class != NULL)
	{
		PauseWidgetClass = pauseClass.Class;
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

	PlayerController = this->GetOwningPlayerController();

	// hud
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

	// bindings
	if (AOsGameMode* osGameMode = Cast<AOsGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		osGameMode->OnGameOver.AddUniqueDynamic(this, &AOsHUD::HandleGameOver);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to do binfdings in AOsHUD::BeginPlay()"));
	}

	if (AOsPlayerController* osPlayerController = Cast<AOsPlayerController>(PlayerController))
	{
		osPlayerController->OnPause.AddUniqueDynamic(this, &AOsHUD::HandleGamePaused);
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

void AOsHUD::ShowStartingWidget(bool showCursor)
{
	ChangeMenuWidget(StartingWidgetClass, showCursor);
}

void AOsHUD::ShowEndingWidget(bool showCursor)
{
	ChangeMenuWidget(EndingWidgetClass, showCursor);
}

void AOsHUD::ShowPauseWidget(bool showCursor)
{
	ChangeMenuWidget(PauseWidgetClass, showCursor);
}

void AOsHUD::ShowHUDWidget(bool showCursor)
{
	ChangeMenuWidget(HUDWidgetClass, showCursor);
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

void AOsHUD::HandleGameOver(EGameResult const gameResult)
{
	ShowEndingWidget();
}

void AOsHUD::HandleGamePaused(bool const bIsPaused)
{
	if (bIsPaused)
	{
		ShowPauseWidget();
	}
	else
	{
		ChangeMenuWidget(OldWidgetClass, false);
	}
}
