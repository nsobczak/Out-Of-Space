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
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category="Crosshair")
	void Lock(bool const bLock = true);

protected:
	UPROPERTY(Transient)
	class AOsPlayerController* OsPlayerController;

	UPROPERTY(VisibleInstanceOnly, Category="Crosshair", Transient)
	bool bIsLocked = false;

	void PlayLockAnimation(bool const bReverse = false);

private:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* LockAnimation;
};
