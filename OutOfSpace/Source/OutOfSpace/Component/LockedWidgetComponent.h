#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "LockedWidgetComponent.generated.h"


UCLASS(ClassGroup="LockCharacters",
	meta=(BlueprintSpawnableComponent, ToolTip="WidgetComponent used to show the attached character is locked"))
class OUTOFSPACE_API ULockedWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	ULockedWidgetComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

	class AOsPlayerCharacter* PlayerCharacter;

	void HandleLockEvent(AActor* LockedBy, bool bIsLocked) { SetHiddenInGame(!bIsLocked); } ;
	UFUNCTION()
	void HandleLock(AActor* LockedBy) { HandleLockEvent(LockedBy, true); };
	UFUNCTION()
	void HandleUnlock(AActor* LockedBy) { HandleLockEvent(LockedBy, false); };
};
