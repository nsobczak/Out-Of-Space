#pragma once

#include "CoreMinimal.h"
#include "OutOfSpace/UI/OsWidget.h"
#include "W_Crosshair.generated.h"

/**
 * 
 */
UCLASS()
class OUTOFSPACE_API UW_Crosshair : public UOsWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Crosshair")
	void PlayLockAnimation(bool const bReverse = false);

private:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* LockAnimation;
};
