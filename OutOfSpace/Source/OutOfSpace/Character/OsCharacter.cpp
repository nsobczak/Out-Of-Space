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

//////////////////////////////////////////////////////////////////////////
// Input

// void AOsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
// {
// 	// Set up gameplay key bindings
// 	check(PlayerInputComponent);
// 	PlayerInputComponent->BindAction("RollRight", IE_Pressed, this, &ACharacter::Jump);
// 	PlayerInputComponent->BindAction("RollLeft", IE_Released, this, &ACharacter::StopJumping);
// 	PlayerInputComponent->BindAction("Lock", IE_Pressed, this, &ACharacter::StopJumping);
// 	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACharacter::StopJumping);
// 	
// 	PlayerInputComponent->BindAxis("MoveForward", this, &AOsCharacter::MoveForward);
// 	PlayerInputComponent->BindAxis("MoveRight", this, &AOsCharacter::MoveRight);
//
// 	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
// 	// "turn" handles devices that provide an absolute delta, such as a mouse.
// 	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
// 	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
// 	PlayerInputComponent->BindAxis("TurnRate", this, &AOsCharacter::TurnAtRate);
// 	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
// 	PlayerInputComponent->BindAxis("LookUpRate", this, &AOsCharacter::LookUpAtRate);
//
// 	// handle touch devices
// 	PlayerInputComponent->BindTouch(IE_Pressed, this, &AOsCharacter::TouchStarted);
// 	PlayerInputComponent->BindTouch(IE_Released, this, &AOsCharacter::TouchStopped);
// }


// void AOsCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
// {
// 		Jump();
// }
//
// void AOsCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
// {
// 		StopJumping();
// }
//
// void AOsCharacter::TurnAtRate(float Rate)
// {
// 	// calculate delta for this frame from the rate information
// 	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
// }
//
// void AOsCharacter::LookUpAtRate(float Rate)
// {
// 	// calculate delta for this frame from the rate information
// 	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
// }
//
// void AOsCharacter::MoveForward(float Value)
// {
// 	if ((Controller != nullptr) && (Value != 0.0f))
// 	{
// 		// find out which way is forward
// 		const FRotator Rotation = Controller->GetControlRotation();
// 		const FRotator YawRotation(0, Rotation.Yaw, 0);
//
// 		// get forward vector
// 		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
// 		AddMovementInput(Direction, Value);
// 	}
// }
//
// void AOsCharacter::MoveRight(float Value)
// {
// 	if ( (Controller != nullptr) && (Value != 0.0f) )
// 	{
// 		// find out which way is right
// 		const FRotator Rotation = Controller->GetControlRotation();
// 		const FRotator YawRotation(0, Rotation.Yaw, 0);
// 	
// 		// get right vector 
// 		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
// 		// add movement in that direction
// 		AddMovementInput(Direction, Value);
// 	}
// }

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


float AOsCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                               AActor* DamageCauser)
{
	float damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (DamageCauser)
	{
		UE_LOG(LogOoS, Log, TEXT("%s damages %s"), *DamageCauser->GetName(), *GetName());
	}

	if (HealthComp)
	{
		HealthComp->AddHealth(-damage);
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
	UE_LOG(LogOoS, Log, TEXT("kill %s"), *GetName());

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
