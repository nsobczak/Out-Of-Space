#include "StaminaComponent.h"


UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentStamina = MaxStamina;
}


// void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType,
// 	FActorComponentTickFunction* ThisTickFunction)
// {
// 	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
// 	// ...
// }

void UStaminaComponent::AddStamina(float staminaAmount)
{
	CurrentStamina += staminaAmount;

	CurrentStamina = FMath::Clamp(CurrentStamina, 0.f, MaxStamina);
}
