#include "OsCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "OutOfSpace/OutOfSpace.h"
#include "OutOfSpace/Component/HealthComponent.h"
#include "OutOfSpace/Component/LockControllerComponent.h"
#include "OutOfSpace/Component/LockedWidgetComponent.h"
#include "OutOfSpace/Projectile/ProjectileBase.h"
#include "OutOfSpace/Projectile/ProjectileHoming.h"

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
	HomingProjectileClass = AProjectileHoming::StaticClass();

	MuzzleComp = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	MuzzleComp->SetupAttachment(RootComponent);
	MuzzleComp->SetRelativeLocation(FVector(50.f, 0, 120.f));

	LockedWidget = CreateDefaultSubobject<ULockedWidgetComponent>(TEXT("LockedWidget"));
	LockedWidget->SetupAttachment(RootComponent);

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

void AOsCharacter::FireSimple(FVector MuzzleLocation, FRotator MuzzleRotation, UWorld* World,
	FActorSpawnParameters SpawnParams)
{
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to fire a projectile: ProjectileClass is null"));
		return;
	}

	AProjectileBase* Projectile = World->SpawnActor<AProjectileBase>(ProjectileClass, MuzzleLocation,
		MuzzleRotation, SpawnParams);
	if (Projectile)
	{
		FVector LaunchDirection = IsPlayerControlled() ? MuzzleRotation.Vector() : GetActorForwardVector();
		Projectile->FireInDirection(LaunchDirection);
	}
}

void AOsCharacter::FireOnLocked(FVector MuzzleLocation, FRotator MuzzleRotation, UWorld* World,
	FActorSpawnParameters SpawnParams)
{
	UE_LOG(LogOoS, Log, TEXT("AOsCharacter::FireOnLocked"));

	if (!HomingProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to fire a projectile: HomingProjectileClass is null"));
		return;
	}

	ULockControllerComponent* lockController = Cast<ULockControllerComponent>(
		GetComponentByClass(ULockControllerComponent::StaticClass()));
	if (!lockController) { return; }

	for (int i = 0; i < lockController->GetLockedCharacters().Num(); ++i)
	{
		AProjectileHoming* Projectile = World->SpawnActor<AProjectileHoming>(HomingProjectileClass, MuzzleLocation,
			MuzzleRotation, SpawnParams);
		if (Projectile)
		{
			FVector LaunchDirection = IsPlayerControlled() ? MuzzleRotation.Vector() : GetActorForwardVector();
			Projectile->FireInDirectionToTarget(LaunchDirection, lockController->GetLockedCharacters()[i]);
		}
	}
}

void AOsCharacter::Fire(EFireType fireType)
{
	// Attempt to fire a projectile.
	// TODO: add timer to prevent character from firing too often, right it is done manually in behavior tree for foes


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


		switch (fireType)
		{
		case EFireType::FT_ON_LOCKED:
			{
				FireOnLocked(MuzzleLocation, MuzzleRotation, World, SpawnParams);
			}
		case EFireType::FT_SIMPLE:
		default:
			{
				FireSimple(MuzzleLocation, MuzzleRotation, World, SpawnParams);
			}
		}
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
