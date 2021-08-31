#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OUTOFSPACE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	// Initial and max health
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth = 100.f;

protected:
	virtual void BeginPlay() override;

	// Set to MaxHealth on begin play
	UPROPERTY(VisibleAnywhere, Category = "Health")
	float CurrentHealth = 1.f;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category="Health")
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; };

	UFUNCTION(BlueprintCallable, Category="Health")
	FORCEINLINE void ResetHealth() { CurrentHealth = MaxHealth; };

	UFUNCTION(Exec, BlueprintCallable, Category="Health")
	void AddHealth(float healthAmount);

	UFUNCTION(BlueprintPure, Category="Health")
	FORCEINLINE bool IsDead() const { return CurrentHealth <= 0; };
};
