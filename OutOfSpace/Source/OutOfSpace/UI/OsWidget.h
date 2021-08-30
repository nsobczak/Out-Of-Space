

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OsWidget.generated.h"

/**
 * 
 */
UCLASS()
class OUTOFSPACE_API UOsWidget : public UUserWidget
{
	GENERATED_BODY()

	public:
	UOsWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeConstruct() override;

	// virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//
	// virtual void NativeDestruct() override;
};
