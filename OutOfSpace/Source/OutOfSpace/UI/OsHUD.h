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
	
	UFUNCTION(BlueprintPure, Category = "HUD")
	static FString GetProjectVersion();

protected:
	/** Called when the game starts. */
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleGameOver(EGameResult const gameResult);

	UFUNCTION()
	void HandleGamePaused(bool const bIsPaused);

private:
	UPROPERTY(Transient)
	APlayerController* PlayerController;

#pragma region Widget
public:
	UFUNCTION(BlueprintCallable, Category = "Widget_Functions")
	void ShowCursor(bool showCursor);

	/** The widget instance that we are using. */
	UFUNCTION(BlueprintPure, Category = "Widget_Functions")
	FORCEINLINE UUserWidget* GetCurrentWidget() { return CurrentWidget; }

	UFUNCTION(BlueprintPure, Category = "Widget_Functions")
	FORCEINLINE TSubclassOf<UUserWidget> GetCurrentWidgetClass() { return CurrentWidgetClass; }

	/** Remove the current menu widget (if not null) and create a new one from the specified class, if provided. Show cursor if true.*/
	UFUNCTION(BlueprintCallable, Category = "Widget_Functions")
	UUserWidget* ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass, bool showCursor = false);

	UFUNCTION(BlueprintPure, Category = "Widget_Functions")
	bool IsShowingWidget(EHudWidget widgetToLookFor);

	UFUNCTION(BlueprintCallable, Category = "Widget_Functions")
	void ShowWidget(TSubclassOf<UUserWidget> NewWidgetClass, bool showCursor = false);

	UFUNCTION(BlueprintCallable, Category = "Widget_Functions")
	void ShowHUDWidget(bool showCursor = false);

	UFUNCTION(BlueprintCallable, Category = "Widget_Functions")
	void ShowStartingWidget(bool showCursor = true);

	UFUNCTION(BlueprintCallable, Category = "Widget_Functions")
	void ShowEndingWidget(bool showCursor = true);

	UFUNCTION(BlueprintCallable, Category = "Widget_Functions")
	void ShowPauseWidget(bool showCursor = true);

	UFUNCTION(BlueprintCallable, Category = "Widget_Functions")
	void HideWidget(UUserWidget* widgetToHide, bool showCursor = false);

	UFUNCTION(BlueprintCallable, Category = "Widget_Functions")
	void HideCurrentWidget(bool showCursor = false);

protected:
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets", Transient)
	UUserWidget* CurrentWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> CurrentWidgetClass;

	/** The widget instance that we used. (useful for return buttons) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> OldWidgetClass;
#pragma endregion
};
