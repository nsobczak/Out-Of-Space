#include "OutOfSpace/UI/OsWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
// #include "Components/TextBlock.h"
#include "OutOfSpace/OutOfSpace.h"

#define LOCTEXT_NAMESPACE "OsUMG"

UOsWidget::UOsWidget(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

TSharedRef<SWidget> UOsWidget::RebuildWidget()
{
	RootWidget = Cast<UPanelWidget>(GetRootWidget());

	if (!RootWidget)
	{
		RootWidget = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(),TEXT("CanvasPanel"));
		UCanvasPanelSlot* rootWidgetSlot = Cast<UCanvasPanelSlot>(RootWidget->Slot);
		if (rootWidgetSlot)
		{
			rootWidgetSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			rootWidgetSlot->SetOffsets(FMargin(0.f, 0.f));
		}

		WidgetTree->RootWidget = RootWidget;
	}

	TSharedRef<SWidget> widget = Super::RebuildWidget();

	// // Exemple
	// if (RootWidget && WidgetTree)
	// {
	// 	TextBlock_Test = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TextBlock_Test"));
	// 	RootWidget->AddChild(TextBlock_Test);
	// 	TextBlock_Test->SetText(FText::FromString("test"));
	//
	// 	UCanvasPanelSlot* TextBlock_TestSlot = Cast<UCanvasPanelSlot>(TextBlock_Test->Slot);
	// 	if (TextBlock_TestSlot)
	// 	{
	// 		TextBlock_TestSlot->SetAutoSize(true);
	// 	}
	// }

	return widget;
}

#if WITH_EDITOR
const FText UOsWidget::GetPaletteCategory()
{
	return LOCTEXT("OsUI", "OsWidget");
}
#endif


void UOsWidget::NativeConstruct()
{
	Super::NativeConstruct();
}
