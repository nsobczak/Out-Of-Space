#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OutOfSpace/Character/OsCharacter.h"
#include "LockControllerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLockControllerUpdatedEvent);

UCLASS(ClassGroup="LockCharacters", meta=(BlueprintSpawnableComponent, ToolTip="Controller to lock other characters"))
class OUTOFSPACE_API ULockControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULockControllerComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category="Lock")
	FORCEINLINE TArray<AOsCharacter*> GetLockedCharacters() const { return LockedCharacters; };

	UFUNCTION(BlueprintPure, Category="Lock")
	FORCEINLINE int32 GetLockedCharacterCount() const { return LockedCharacters.Num(); };

	bool LockCharacter(AOsCharacter*& osCharacterToLock);

	void ReleaseLock(bool bWasLockCanceled = false);

	UFUNCTION(BlueprintPure, Category="Lock")
	FORCEINLINE int32 GetLockMaxCount() const { return LockMaxCount; };

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Lock")
	FLockControllerUpdatedEvent OnLockedCharactersUpdated;

protected:
	// virtual void BeginPlay() override;

	// Max number of characters that can be locked simultaneously
	UPROPERTY(EditAnywhere, Category="Lock")
	int32 LockMaxCount = 5;

	// OsCharactersCurrently locked
	UPROPERTY(VisibleInstanceOnly, Category="Lock")
	TArray<AOsCharacter*> LockedCharacters;

	// Whether we can add a given character to the LockedCharacters array
	UFUNCTION(BlueprintPure, Category="Lock")
	bool CanLockCharacter(AOsCharacter*& osCharacterToLock) const;

private:
	// Whether we can add character to the LockedCharacters array
	FORCEINLINE bool CanAddLockedCharacter() const
	{
		return TimeRemainingBlockingLock <= 0 && LockedCharacters.Num() < LockMaxCount;
	};

	// Time between 2 enemy locks
	const float TimeBetweenLock = 0.5f;
	float TimeRemainingBlockingLock;

	const float TimeBetweenLockOnSameTarget = 1.f;
	// Time remaining for a given character
	TMap<AOsCharacter*, float> TimeRemainingBlockingLockOnSameTargetMap;
};
