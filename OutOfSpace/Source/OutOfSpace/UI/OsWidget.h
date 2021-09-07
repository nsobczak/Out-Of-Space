#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OsWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = UI)
class OUTOFSPACE_API UOsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UOsWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	// virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//
	// virtual void NativeDestruct() override;

public:
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="OsWidget")
	UPanelWidget* RootWidget;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="OsWidget")
	// class UTextBlock* TextBlock_Test;
};
