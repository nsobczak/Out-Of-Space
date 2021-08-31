// 

#pragma once

#include "OsCharacter.h"
#include "OsPlayerCharacter.generated.h"

UCLASS()
class OUTOFSPACE_API AOsPlayerCharacter : public AOsCharacter
{
	GENERATED_BODY()

public:
	AOsPlayerCharacter();

protected:
	virtual void BeginPlay() override;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	AController* CurrentController;

public:
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
};
