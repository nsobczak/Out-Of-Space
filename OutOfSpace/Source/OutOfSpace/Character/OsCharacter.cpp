#include "OsCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "OutOfSpace/OutOfSpace.h"
#include "OutOfSpace/Component/HealthComponent.h"
#include "OutOfSpace/Projectile/ProjectileBase.h"

//////////////////////////////////////////////////////////////////////////
// AOsCharacter

AOsCharacter::AOsCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 30.f;
	BaseLookUpRate = 25.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 0;
	GetCharacterMovement()->AirControl = 0.2f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	ProjectileClass = AProjectileBase::StaticClass();

	// muzzle
	MuzzleComp = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	MuzzleComp->SetupAttachment(RootComponent);
	MuzzleComp->SetRelativeLocation(FVector(50.f, 0, 120.f));

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
}

void AOsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CharacterState)
	{
	case ECharacterState::CS_ROLLING:
		{
			HandleRoll(DeltaTime);
		}
	default:
		{
		}
	}
}

void AOsCharacter::Fire()
{
	// Attempt to fire a projectile.
	// TODO: add timer to prevent character from firing too often
	if (ProjectileClass)
	{
		// Get the camera transform.
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

		// Transform MuzzleOffset from camera space to world space.
		// FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
		FVector MuzzleLocation = MuzzleComp->GetComponentLocation();

		// // Skew the aim to be slightly upwards.
		FRotator MuzzleRotation = CameraRotation;
		// MuzzleRotation.Pitch += 10.0f;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			AProjectileBase* Projectile = World->SpawnActor<AProjectileBase>(
				ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = IsPlayerControlled() ? MuzzleRotation.Vector() : GetActorForwardVector();
				Projectile->FireInDirection(LaunchDirection);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to fire a projectile: ProjectileClass is null"));
	}
}

void AOsCharacter::Roll(const bool bIsRollRight)
{
	if (CharacterState != ECharacterState::CS_ROLLING)
	{
		TimeRemainingRolling = RollDuration;
		RollStartLocation = GetActorLocation();
		RollTargetLocation = RollStartLocation + (bIsRollRight ? 1.f : -1.f) * GetActorRightVector() * RollDistance;
		CharacterState = ECharacterState::CS_ROLLING;
	}
}

void AOsCharacter::HandleRoll(float DeltaTime)
{
	TimeRemainingRolling -= DeltaTime;

	float alpha = (RollDuration - TimeRemainingRolling) / RollDuration;
	alpha = FMath::Clamp(alpha, 0.f, 1.f);
	float curveAlpha = alpha;

	if (RollCurve != nullptr)
	{
		curveAlpha = RollCurve->GetFloatValue(alpha);
	}

	SetActorLocation(FMath::Lerp(RollStartLocation, RollTargetLocation, curveAlpha));

	if (TimeRemainingRolling <= 0)
	{
		CharacterState = ECharacterState::CS_DEFAULT;
	}
}


float AOsCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (CharacterState == ECharacterState::CS_DEAD) { return 0; }

	float damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageAmount > 0 && DamageCauser)
	{
		UE_LOG(LogOoS, Log, TEXT("%s damages %s"), *DamageCauser->GetName(), *GetName());
	}


	if (HealthComp)
	{
		HealthComp->Add(-damage);
		if (HealthComp->IsDead())
		{
			if (AOsCharacter* osCharacter = Cast<AOsCharacter>(DamageCauser))
			{
				osCharacter->AddFoeKilledCount(1);
			}
			Kill();
		}
	}

	return damage;
}

void AOsCharacter::Kill()
{
	UE_LOG(LogOoS, Log, TEXT("%s is dead"), *GetName());

	CharacterState = ECharacterState::CS_DEAD;
	OnDeath.Broadcast();

	if (!IsPlayerControlled())
	{
		Destroy();
	}
}

void AOsCharacter::AddFoeKilledCount(int32 const amount)
{
	FoeKilledCount += amount;

	OnFoeKilledCountUpdated.Broadcast(FoeKilledCount);
};
