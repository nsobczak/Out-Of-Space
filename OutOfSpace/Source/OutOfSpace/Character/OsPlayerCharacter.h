#pragma once

#include "OsCharacter.h"
#include "OsPlayerCharacter.generated.h"

UCLASS()
class OUTOFSPACE_API AOsPlayerCharacter : public AOsCharacter
{
	GENERATED_BODY()

public:
	AOsPlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
		AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category="Projectile")
	FORCEINLINE FVector GetCrosshairWorldLocation() const { return CrosshairLocComp->GetComponentLocation(); }

	// virtual void FireSimple(EFireType fireType) override;

protected:
	virtual void BeginPlay() override;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	AController* CurrentController;

	// Aiming point to get location of crosshair
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	USceneComponent* CrosshairLocComp;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float AimingPointDist = 5000.f;

	UPROPERTY(VisibleAnywhere, Category = "Character")
	class UStaminaComponent* StaminaComp;

	UFUNCTION(Exec)
	void KillPlayer();

	UPROPERTY(VisibleInstanceOnly, Category = "Cheat")
	bool bIsInvulnerable = false;

	UFUNCTION(Exec, BlueprintCallable, Category = "Cheat")
	void ToggleInvulnerabilityForPlayer() { bIsInvulnerable = ! bIsInvulnerable; }

	UPROPERTY(VisibleInstanceOnly, Category = "Projectile")
	AActor* CurrentTarget;
	
};
