// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OutOfSpace/Component/HealthComponent.h"
#include "OsCharacter.generated.h"

class UHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBasicEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FValueUpdateEvent, int32 const, newValue);

UCLASS(config=Game)
class AOsCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AOsCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	// Function that handles firing projectiles.
	UFUNCTION()
	void Fire();

	// Gun muzzle offset from the camera location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

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

protected:
	// Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	USceneComponent* MuzzleComp;

	// declare point light comp
	UPROPERTY(VisibleAnywhere)
	UHealthComponent* HealthComp;

	void Kill();

	// Amount of enemy killed
	UPROPERTY(VisibleInstanceOnly, Category = "Gameplay")
	int32 FoeKilledCount = 0;

	// /** Called for forwards/backward input */
	// void MoveForward(float Value);
	//
	// /** Called for side to side input */
	// void MoveRight(float Value);
	//
	// /** 
	//  * Called via input to turn at a given rate. 
	//  * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	//  */
	// void TurnAtRate(float Rate);
	//
	// /**
	//  * Called via input to turn look up/down at a given rate. 
	//  * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	//  */
	// void LookUpAtRate(float Rate);
	//
	// /** Handler for when a touch input begins. */
	// void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	//
	// /** Handler for when a touch input stops. */
	// void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	// protected:
	// 	// APawn interface
	// 	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// 	// End of APawn interface
};
