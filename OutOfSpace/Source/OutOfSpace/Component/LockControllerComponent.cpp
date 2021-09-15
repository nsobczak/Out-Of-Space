#include "LockControllerComponent.h"


ULockControllerComponent::ULockControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void ULockControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


void ULockControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool ULockControllerComponent::LockCharacter(AOsCharacter*& osCharacterToLock)
{
	bool bSuccess = false;
	if (CanAddLockedCharacter())
	{
		LockedCharacters.Add(osCharacterToLock);
		osCharacterToLock->OnLocked.Broadcast(GetOwner());
		bSuccess = true;
	}

	return bSuccess;
}

void ULockControllerComponent::ReleaseLock(bool bWasLockCanceled)
{
	for (int i = 0; i < LockedCharacters.Num(); ++i)
	{
		AOsCharacter*& osChar = LockedCharacters[i];
		// TODO: toggle lock widget visibility
		osChar->OnUnlocked.Broadcast(GetOwner());

		if (!bWasLockCanceled && osChar)
		{
			// TODO: fire homing projectile
		}
	}

	LockedCharacters.Reset();
}
