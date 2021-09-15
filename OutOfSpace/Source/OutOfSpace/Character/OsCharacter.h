// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OutOfSpace/Component/HealthComponent.h"
#include "OutOfSpace/Data/ShootingEnums.h"
#include "OsCharacter.generated.h"

class UHealthComponent;

UENUM(BlueprintType)
enum class EFaction : uint8
{
	F_PLAYER UMETA(DisplayName = "Player ally"),
	F_ALIEN UMETA(DisplayName = "Alien"),
	F_NEUTRAL UMETA(DisplayName = "Neutral"),
	F_OTHER UMETA(DisplayName = "Other")
};

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	CS_DEFAULT UMETA(DisplayName = "Default"),
	CS_ROLLING UMETA(DisplayName = "Rolling"),
	CS_DEAD UMETA(DisplayName = "Dead"),
	CS_OTHER UMETA(DisplayName = "Other")
};

UENUM(BlueprintType)
enum class EFireType : uint8
{
	FT_SIMPLE UMETA(DisplayName = "Default"),
	FT_ON_LOCKED UMETA(DisplayName = "OnLocked")
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBasicEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FValueUpdateEvent, int32 const, newValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLockEvent, AActor*, LockedBy);

UCLASS(config=Game)
class AOsCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AOsCharacter();

	virtual void Tick(float DeltaTime) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	// Handles firing projectiles.
	UFUNCTION()
	virtual void FireInput() { Fire(EFireType::FT_SIMPLE); };
	virtual void Fire(EFireType fireType = EFireType::FT_SIMPLE);

	// Performs strafe dash.
	UFUNCTION()
	virtual void Roll(const bool bIsRollRight);

	UFUNCTION(BlueprintPure, Category = "Character")
	FORCEINLINE EFaction GetFaction() const { return Faction; };

	UFUNCTION(BlueprintPure, Category = "Character")
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; };

	// Gun muzzle offset from the camera location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
		AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category = Gameplay)
	FORCEINLINE UHealthComponent* GetHealthComp() const { return HealthComp; };

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Events")
	FBasicEvent OnDeath;

	UFUNCTION(BlueprintPure, Category = Gameplay)
	FORCEINLINE bool IsDead() const { return HealthComp ? HealthComp->IsDead() : false; };

	UFUNCTION(BlueprintPure, Category = Gameplay)
	FORCEINLINE int32 GetFoeKilledCount() const { return FoeKilledCount; };

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void AddFoeKilledCount(int32 const amount);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Events")
	FValueUpdateEvent OnFoeKilledCountUpdated;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Events")
	FLockEvent OnLocked;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Events")
	FLockEvent OnUnlocked;

protected:
	// Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AProjectileBase> ProjectileClass;

	// Homing projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AProjectileBase> HomingProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	USceneComponent* MuzzleComp;

	// declare point light comp
	UPROPERTY(VisibleAnywhere, Category = "Character")
	UHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	class ULockedWidgetComponent* LockedWidget;

	void Kill();

	// Amount of enemy killed
	UPROPERTY(VisibleInstanceOnly, Category = "Gameplay")
	int32 FoeKilledCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	EFaction Faction = EFaction::F_NEUTRAL;

	UPROPERTY(VisibleInstanceOnly, Category = "Character")
	ECharacterState CharacterState = ECharacterState::CS_DEFAULT;

	void FireSimple(FVector MuzzleLocation, FRotator MuzzleRotation, UWorld* World, FActorSpawnParameters SpawnParams);
	// Handles firing projectiles on locked actors if a LockControllerComponent is equipped.
	void FireOnLocked(FVector MuzzleLocation, FRotator MuzzleRotation, UWorld* World,
		FActorSpawnParameters SpawnParams);

	UPROPERTY(EditDefaultsOnly, Category = "Character|Roll")
	float RollDuration = .5f;

	UPROPERTY(EditDefaultsOnly, Category = "Character|Roll")
	float RollDistance = 700.f;

	// Curve to use on rolling
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Roll")
	UCurveFloat* RollCurve = nullptr;

private:
	FVector RollStartLocation, RollTargetLocation;
	float TimeRemainingRolling;
	UFUNCTION(Category = "Character|Roll")
	void HandleRoll(float DeltaTime);
};
