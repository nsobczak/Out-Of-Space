#include "LockedWidgetComponent.h"

#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "OutOfSpace/OutOfSpace.h"
#include "OutOfSpace/Character/OsPlayerCharacter.h"


ULockedWidgetComponent::ULockedWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> hudClass(
		TEXT("/Game/UI/Hud/LockedCharacter/W_LockedCharacter"));
	if (hudClass.Class != NULL)
	{
		SetWidgetClass(hudClass.Class);
	}

	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetHiddenInGame(true);
}

void ULockedWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<AOsPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!PlayerCharacter)
	{
		UE_LOG(LogOoS, Error, TEXT("failed to retrieve PlayerCharacter in ULockedWidgetComponent::BeginPlay"));
		return;
	}

	AOsCharacter* attachedOsChara = Cast<AOsCharacter>(GetOwner());
	if (attachedOsChara)
	{
		attachedOsChara->OnLocked.AddUniqueDynamic(this, &ULockedWidgetComponent::HandleLock);
		attachedOsChara->OnUnlocked.AddUniqueDynamic(this, &ULockedWidgetComponent::HandleUnlock);
	}
	else
	{
		UE_LOG(LogOoS, Warning, TEXT("Failed to cast GetOwner to AOsCharacter"));
	}
}

void ULockedWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PlayerCharacter)
	{
		// update rotation
		FVector ObserverCameraLocation = PlayerCharacter->GetFollowCamera()->GetComponentLocation();
		FRotator targetRot = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(),
			ObserverCameraLocation);
		SetWorldRotation(targetRot);
	}
}
