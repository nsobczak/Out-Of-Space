// 

#pragma once

#include "OsCharacter.h"
#include "OsPlayerCharacter.generated.h"

UCLASS()
class OUTOFSPACE_API AOsPlayerCharacter : public AOsCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOsPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
