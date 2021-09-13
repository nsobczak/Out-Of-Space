#include "W_DamageIndicatorSpawner.h"

#include "W_DamageIndicatorElt.h"
#include "OutOfSpace/Character/OsPlayerCharacter.h"

UW_DamageIndicatorSpawner::UW_DamageIndicatorSpawner(const FObjectInitializer& ObjectInitializer): Super(
	ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> dmgIndicatorEltClass(
		TEXT("/Game/UI/Hud/DamageIndicator/W_DamageIndicator_BP"));
	if (dmgIndicatorEltClass.Class != NULL)
	{
		DamageIndicatorEltClass = dmgIndicatorEltClass.Class;
	}
}

void UW_DamageIndicatorSpawner::NativeConstruct()

{
	Super::NativeConstruct();

	OsPlayerCharacter = Cast<AOsPlayerCharacter>(GetOwningPlayerPawn());
	if (OsPlayerCharacter)
	{
		OsPlayerCharacter->OnTakeAnyDamage.AddUniqueDynamic(this, &UW_DamageIndicatorSpawner::SpawnDamageIndicatorElt);
	}
}

// void UW_DamageIndicatorSpawner::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
// {
// 	Super::NativeTick(MyGeometry, InDeltaTime);
//
// }

void UW_DamageIndicatorSpawner::NativeDestruct()
{
	Super::NativeDestruct();

	if (OsPlayerCharacter)
	{
		OsPlayerCharacter->OnTakeAnyDamage.RemoveDynamic(this, &UW_DamageIndicatorSpawner::SpawnDamageIndicatorElt);
	}
}

void UW_DamageIndicatorSpawner::SpawnDamageIndicatorElt(AActor* DamagedActor, float Damage,
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UW_DamageIndicatorElt* currentElt = CreateWidget<UW_DamageIndicatorElt>(GetWorld(), DamageIndicatorEltClass);
	if (currentElt && DamageCauser)
	{
		currentElt->SetLifeTime(EltLifeTime);
		currentElt->SetDamageCauser(DamageCauser);
		currentElt->AddToViewport();
	}
}
