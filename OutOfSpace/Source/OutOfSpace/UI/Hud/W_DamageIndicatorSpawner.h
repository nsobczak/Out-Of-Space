#pragma once

#include "CoreMinimal.h"
#include "OutOfSpace/UI/OsWidget.h"
#include "UObject/Object.h"
#include "W_DamageIndicatorSpawner.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class OUTOFSPACE_API UW_DamageIndicatorSpawner : public UOsWidget
{
	GENERATED_BODY()

public:
	UW_DamageIndicatorSpawner(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	// virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DamageIndicator")
	TSubclassOf<class UW_DamageIndicatorElt> DamageIndicatorEltClass;

	// Life time of a damage indicator element
	UPROPERTY(EditDefaultsOnly, Category="DamageIndicator")
	float EltLifeTime = 3.f;

protected:
	UPROPERTY(Transient)
	class AOsPlayerCharacter* OsPlayerCharacter;

	UFUNCTION()
	void SpawnDamageIndicatorElt(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);
};
