#pragma once

#include "CoreMinimal.h"
#include "ProgressBarComponent.h"
#include "HealthComponent.generated.h"


UCLASS(ClassGroup=(ProgressBar), meta=(BlueprintSpawnableComponent))
class OUTOFSPACE_API UHealthComponent : public UProgressBarComponent
{
	GENERATED_BODY()

public:
	// UHealthComponent();

	// virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	//                            FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category="Health")
	FORCEINLINE bool IsDead() const { return IsEmpty(); };
	
// protected:
// 	virtual void BeginPlay() override;

};
