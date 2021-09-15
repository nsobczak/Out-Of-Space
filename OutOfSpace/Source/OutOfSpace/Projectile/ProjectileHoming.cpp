#include "ProjectileHoming.h"

#include "GameFramework/ProjectileMovementComponent.h"


AProjectileHoming::AProjectileHoming()
{
	PrimaryActorTick.bCanEverTick = true;

	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->bIsHomingProjectile = true;
		ProjectileMovementComponent->HomingAccelerationMagnitude = 4000.f;
		ProjectileMovementComponent->Velocity = FVector::ZeroVector;
	}

	ProjectileType = EProjectileType::Homing;
	DamageValue = 20.f;
}

void AProjectileHoming::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileHoming::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileHoming::FireInDirectionToTarget(const FVector& ShootDirection, AActor* HomingTarget)
{
	if (HomingTarget)
	{
		ProjectileMovementComponent->HomingTargetComponent = HomingTarget->GetRootComponent();
	}

	FireInDirection(ShootDirection);
}
