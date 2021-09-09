#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OsPlayerController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMoveEvent, float, value);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBasicActionEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPauseEvent, bool, bIsPaused);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRollEvent, bool, bIsRollRight);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPawnEvent, APawn*, pawn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCrosshairEvent, bool, bIsLocked);


/**
 * 
 */
UCLASS()
class OUTOFSPACE_API AOsPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AOsPlayerController(const FObjectInitializer& ObjInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnPossess(APawn* InPawn) override;
	// virtual void OnUnPossess() override;

	virtual void Pause() override;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inputs")
	FPawnEvent OnNewPawnPossessed;

	UFUNCTION()
	void HandleIsGamePlayingUpdated(bool newVal);

	UFUNCTION(BlueprintPure, Category="game")
	float GetGoalCompletion() const;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Projectile")
	FCrosshairEvent OnCrosshairLock;

	UFUNCTION(BlueprintPure, Category="Projectile")
	FORCEINLINE FVector2D GetCrosshairScreenLocation() const { return CrosshairScreenLocation; };


protected:
	class AOsGameMode* OsGameMode;
	class AOsPlayerCharacter* OsPlayerCharacter;
	class AOsWorldSettings* OsWorldSettings;

	FVector2D CrosshairScreenLocation;

	UFUNCTION(BlueprintPure, Category="Projectile")
	bool UpdateCrosshairScreenLocation(FVector2D& screenLocation) const;

	// Detect enemies, decide weather to lock crosshair or not, broadcast if lock state changed
	void UpdateCrosshair();
	// Get input keys associated to input actions to check if forward key is held down and if we should move forward
	void HandleMoveForward();

	// SphereTrace to detect enemies
	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	float SphereTraceLengthDetection = 14000.f;
	// SphereTrace to detect enemies
	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	float SphereTraceSphereRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	float ScreenDistanceDetectionThreshold = 400.f;

	UPROPERTY(VisibleInstanceOnly, Category="Inputs")
	bool bArePlayerActionsAllowed = false;


private:
	// used for LookUpAtRate and turn 
	float DefaultBaseRate = 35.f;

	bool bIsCrosshairLocked = false;

#pragma region Inputs
	//______________________________________________________________

public:
	virtual void SetupInputComponent() override;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inputs")
	FBasicActionEvent OnMoveForward;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inputs")
	FBasicActionEvent OnStart;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inputs")
	FBasicActionEvent OnAccept;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inputs")
	FBasicActionEvent OnBack;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inputs")
	FPauseEvent OnPause;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inputs")
	FBasicActionEvent OnFire;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inputs")
	FRollEvent OnRoll;

protected:
	const FName Axis_Turn = "Turn";
	const FName Axis_TurnRate = "TurnRate";
	const FName Axis_LookUp = "LookUp";
	const FName Axis_LookUpRate = "LookUpRate";

	const FName Action_MoveForward = "MoveForward";
	const FName Action_Start = "Start";
	const FName Action_Accept = "Accept";
	const FName Action_Back = "Back";

	const FName Action_RollLeft = "RollLeft";
	const FName Action_RollRight = "RollRight";
	const FName Action_Fire = "Fire";
	const FName Action_Lock = "Lock";

	/** Called for forwards/backward input */
	void MoveForward();

	virtual void AddYawInput(float Val) override;

	/** 
	* Called via input to turn at a given rate. 
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	virtual void AddPitchInput(float Val) override;

	/**
	* Called via input to turn look up/down at a given rate. 
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void Start();
	void Accept();
	void Back();

	void RollLeft();
	void RollRight();
	void Fire();

	void LockStart();
	void LockCancel();
	//____________________________________________________________________________
#pragma endregion
};
