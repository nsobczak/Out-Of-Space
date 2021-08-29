#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OsPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class OUTOFSPACE_API AOsPlayerController : public APlayerController
{
	GENERATED_BODY()

	// TODO: add default constructor
public:
	AOsPlayerController(const FObjectInitializer& ObjInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnPossess(APawn* InPawn) override;
	// virtual void OnUnPossess() override;

	virtual void SetupInputComponent() override;

protected:
	class AOsCharacter* OsPawn;
	
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	* Called via input to turn at a given rate. 
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

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
	void Lock();
	void Fire();

	private:
	// used for LookUpAtRate and turn 
	float DefaultBaseRate = 45.f;
};
