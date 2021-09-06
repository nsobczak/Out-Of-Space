#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}


// void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType,
//                                      FActorComponentTickFunction* ThisTickFunction)
// {
// 	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
// 	// ...
// }

void UHealthComponent::AddHealth(float healthAmount)
{
	CurrentHealth += healthAmount;

	CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);
}
