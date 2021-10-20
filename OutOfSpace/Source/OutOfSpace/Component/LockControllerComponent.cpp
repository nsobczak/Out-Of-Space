#include "LockControllerComponent.h"


ULockControllerComponent::ULockControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = FMath::Min(.1f, TimeBetweenLock); //075f TimeBetweenLock
}


// void ULockControllerComponent::BeginPlay()
// {
// 	Super::BeginPlay();
//
// 	// ...
// }

void ULockControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TimeRemainingBlockingLock > 0)
	{
		TimeRemainingBlockingLock -= DeltaTime;
	}

	// handle lock on same chara
	if (TimeRemainingBlockingLockOnSameTargetMap.Num() > 0)
	{
		TArray<AOsCharacter*> keys;
		TimeRemainingBlockingLockOnSameTargetMap.GetKeys(keys);
		for (int i = 0; i < keys.Num(); ++i)
		{
			float timeRemaining = *TimeRemainingBlockingLockOnSameTargetMap.Find(keys[i]);
			if (timeRemaining > 0)
			{
				TimeRemainingBlockingLockOnSameTargetMap.Emplace(keys[i], timeRemaining - DeltaTime);
			}
		}
	}
}

bool ULockControllerComponent::CanLockCharacter(AOsCharacter*& osCharacterToLock) const
{
	if (TimeRemainingBlockingLockOnSameTargetMap.Contains(osCharacterToLock))
		// LockedCharacters.Contains(osCharacterToLock) && 
	{
		return *TimeRemainingBlockingLockOnSameTargetMap.Find(osCharacterToLock) <= 0 && CanAddLockedCharacter();
	}
	else
	{
		return CanAddLockedCharacter();
	}
};

bool ULockControllerComponent::LockCharacter(AOsCharacter*& osCharacterToLock)
{
	bool bSuccess = false;

	if (CanLockCharacter(osCharacterToLock))
	{
		LockedCharacters.Add(osCharacterToLock);
		osCharacterToLock->OnLocked.Broadcast(GetOwner());

		TimeRemainingBlockingLock = TimeBetweenLock;
		TimeRemainingBlockingLockOnSameTargetMap.FindOrAdd(osCharacterToLock, TimeBetweenLockOnSameTarget);
		OnLockedCharactersUpdated.Broadcast();

		bSuccess = true;
	}

	return bSuccess;
}

void ULockControllerComponent::ReleaseLock(bool bWasLockCanceled)
{
	if (!bWasLockCanceled)
	{
		for (int i = 0; i < LockedCharacters.Num(); ++i)
		{
			AOsCharacter*& osChar = LockedCharacters[i];
			osChar->OnUnlocked.Broadcast(GetOwner());
		}
	}

	LockedCharacters.Reset();
	OnLockedCharactersUpdated.Broadcast();
	TimeRemainingBlockingLock = 0;

	TimeRemainingBlockingLockOnSameTargetMap.Reset();
}
