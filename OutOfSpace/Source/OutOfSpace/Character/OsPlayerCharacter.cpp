#include "OsPlayerCharacter.h"

#include "DrawDebugHelpers.h"
#include "OsPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "OutOfSpace/OutOfSpace.h"
#include "OutOfSpace/Component/StaminaComponent.h"
#include "OutOfSpace/Projectile/ProjectileHoming.h"


AOsPlayerCharacter::AOsPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 600.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 10.f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 20.f;
	CameraBoom->CameraLagMaxDistance = 200.f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm

	CrosshairLocComp = CreateDefaultSubobject<USceneComponent>(TEXT("CrosshairLocation"));
	CrosshairLocComp->SetupAttachment(RootComponent);
	CrosshairLocComp->
		SetRelativeLocation(MuzzleComp->GetRelativeLocation() + AimingPointDist * GetActorForwardVector());

	if (HealthComp)
	{
		HealthComp->MaxValue = 500.f;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComp is null in AOsPlayerCharacter::AOsPlayerCharacter"));
	}

	StaminaComp = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComp"));

	Faction = EFaction::F_PLAYER;

	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
}

void AOsPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	CurrentController = NewController;

	AOsPlayerController* osPController = Cast<AOsPlayerController>(NewController);
	if (osPController)
	{
		osPController->OnFire.AddUniqueDynamic(this, &AOsPlayerCharacter::Fire);
		osPController->OnRoll.AddUniqueDynamic(this, &AOsPlayerCharacter::Roll);
	}
}

void AOsPlayerCharacter::UnPossessed()
{
	Super::UnPossessed();

	if (AOsPlayerController* osPController = Cast<AOsPlayerController>(CurrentController))
	{
		osPController->OnFire.RemoveDynamic(this, &AOsPlayerCharacter::Fire);
	}

	// CurrentController = nullptr;
}

void AOsPlayerCharacter::KillPlayer()
{
	if (HealthComp)
	{
		TakeDamage(HealthComp->MaxValue, FDamageEvent(), GetController(), this);
	}
}

void AOsPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(this, AOsCharacter::StaticClass(), actors);
	if (actors.Num() > 0)
	{
		CurrentTarget = actors[0];
	}
}

void AOsPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOsPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AOsPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	return bIsInvulnerable ? 0 : Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AOsPlayerCharacter::Fire()
{
	// We use this func to test homing projectile
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
			AProjectileHoming* Projectile = World->SpawnActor<AProjectileHoming>(
				ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = IsPlayerControlled() ? MuzzleRotation.Vector() : GetActorForwardVector();
				Projectile->FireInDirectionToTarget(LaunchDirection, CurrentTarget);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to fire a projectile: ProjectileClass is null"));
	}
}