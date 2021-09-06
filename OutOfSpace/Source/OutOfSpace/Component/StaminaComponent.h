#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OUTOFSPACE_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStaminaComponent();

	// Initial and max health
	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float MaxStamina = 100.f;

	// Get current health and divide by health max
	UFUNCTION(BlueprintPure, Category = Gameplay)
	FORCEINLINE float GetStaminaRatio() const { return GetCurrentStamina() / MaxStamina; };

protected:
	virtual void BeginPlay() override;

	// Set to MaxStamina on begin play
	UPROPERTY(VisibleAnywhere, Category = "Stamina")
	float CurrentStamina = 1.f;

public:
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	// 						FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category="Stamina")
	FORCEINLINE float GetCurrentStamina() const { return CurrentStamina; };

	UFUNCTION(BlueprintCallable, Category="Stamina")
	FORCEINLINE void ResetStamina() { CurrentStamina = MaxStamina; };

	UFUNCTION(Exec, BlueprintCallable, Category="Stamina")
	void AddStamina(float staminaAmount);

	UFUNCTION(BlueprintPure, Category="Stamina")
	FORCEINLINE bool IsEmpty() const { return CurrentStamina <= 0; };
};
