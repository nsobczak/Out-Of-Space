#include "LockedWidgetComponent.h"

#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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
}

void ULockedWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<AOsPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("failed to retrieve PlayerCharacter in ULockedWidgetComponent::BeginPlay"));
		return;
	}
	else
	{
		// TODO: get player and bind visibility on lock event
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
