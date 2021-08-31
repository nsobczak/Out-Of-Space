#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OsHUD.generated.h"

enum class EGameResult : uint8;

UENUM(BlueprintType)
enum class EHudWidget : uint8
{
	FHW_NONE UMETA(DisplayName = "Null"),
	FHW_START UMETA(DisplayName = "Start widget"),
	FHW_HUD UMETA(DisplayName = "HUD widget"),
	FHW_PAUSE UMETA(DisplayName = "Pause widget"),
	FHW_END UMETA(DisplayName = "End widget")
};

class UOsWidget;
/**
 * 
 */
UCLASS()
class OUTOFSPACE_API AOsHUD : public AHUD
{
	GENERATED_BODY()

	AOsHUD();

public:
	// // Primary draw call for the HUD.
	// virtual void DrawHUD() override;

#pragma region widget functions
	UFUNCTION(BlueprintCallable, Category = "Widget_Functions")
	void ShowCursor(bool showCursor);

	/** The widget instance that we are using. */
	UFUNCTION(BlueprintPure, Category = "Widget_Functions", Meta = (BlueprintProtected = "true"))
	FORCEINLINE UUserWidget* GetCurrentWidget() { return CurrentWidget; }

	UFUNCTION(BlueprintPure, Category = "Widget_Functions", Meta = (BlueprintProtected = "true"))
	FORCEINLINE TSubclassOf<UUserWidget> GetCurrentWidgetClass() { return CurrentWidgetClass; }

	/** Remove the current menu widget (if not null) and create a new one from the specified class, if provided. Show cursor if true.*/
	UFUNCTION(BlueprintCallable, Category = "Widget_Functions", Meta = (BlueprintProtected = "true"))
	UUserWidget* ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass, bool showCursor = false);

	UFUNCTION(BlueprintPure, Category = "Widget_Functions", Meta = (BlueprintProtected = "true"))
	bool IsShowingWidget(EHudWidget widgetToLookFor);

	UFUNCTION(BlueprintCallable, Category = "Widget_Functions", Meta = (BlueprintProtected = "true"))
	void ShowWidget(TSubclassOf<UUserWidget> NewWidgetClass, bool showCursor = false);

	UFUNCTION(BlueprintCallable, Category = "Widget_Functions", Meta = (BlueprintProtected = "true"))
	void ShowHUDWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget_Functions", Meta = (BlueprintProtected = "true"))
	void ShowStartingWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget_Functions", Meta = (BlueprintProtected = "true"))
	void ShowEndingWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget_Functions", Meta = (BlueprintProtected = "true"))
	void ShowPauseWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget_Functions", Meta = (BlueprintProtected = "true"))
	void HideWidget(UUserWidget* widgetToHide, bool showCursor = false);

	UFUNCTION(BlueprintCallable, Category = "Widget_Functions", Meta = (BlueprintProtected = "true"))
	void HideCurrentWidget(bool showCursor = false);
#pragma endregion


protected:
	/** Called when the game starts. */
	virtual void BeginPlay() override;

#pragma region Widgets
	/** The widget class we will use as our menu when the game starts. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	EHudWidget WidgetClassToShow = EHudWidget::FHW_HUD;

	/** The widget class we will use as our menu when the game starts. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> StartingWidgetClass;

	/**widget to use for our HUD screen*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	/** The widget class we will use as our menu when the game is paused. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> PauseWidgetClass;

	/** The widget class we will use as our menu when the game end. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> EndingWidgetClass;

	/** The widget instance that we are using as our menu. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	UUserWidget* CurrentWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> CurrentWidgetClass;

	/** The widget instance that we used. (useful for return buttons) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> OldWidgetClass;

#pragma endregion

	UFUNCTION()
	void HandleGameOver(EGameResult const gameResult);

	
private:
	UPROPERTY()
	APlayerController* PlayerController;
};
