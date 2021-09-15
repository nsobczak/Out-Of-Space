#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "ProjectileHoming.generated.h"

UCLASS()
class OUTOFSPACE_API AProjectileHoming : public AProjectileBase
{
	GENERATED_BODY()

public:
	AProjectileHoming();
	virtual void Tick(float DeltaTime) override;

	void FireInDirectionToTarget(const FVector& ShootDirection, AActor* HomingTarget);
	
protected:
	virtual void BeginPlay() override;
};
