#include "OutOfSpace/UI/OsWidget.h"

#include "Blueprint/WidgetTree.h"
#include "OutOfSpace/OutOfSpace.h"

UOsWidget::UOsWidget(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}


void UOsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// if (!IsVisible()) { return; }
	//
	// // Set Up Root Widget
	// UWidget* RootWidget = Cast<UWidget>(GetRootWidget());
	// if (!RootWidget)
	// {
	// 	UE_LOG(LogHUD, Error, TEXT("RootWidget is null"));
	// }
	// else
	// {
	// 	//assign properties
	// 	TArray<UWidget*> childWidgets;
	// 	WidgetTree->GetChildWidgets(RootWidget, childWidgets);
	// 	for (size_t i = 0; i < childWidgets.Num(); ++i)
	// 	{
	// 		UWidget* currentWidget = childWidgets[i];
	// 	}
	// }
}
