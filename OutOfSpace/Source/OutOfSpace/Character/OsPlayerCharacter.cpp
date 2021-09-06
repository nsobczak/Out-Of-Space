#include "OsPlayerCharacter.h"

#include "DrawDebugHelpers.h"
#include "OsPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "OutOfSpace/OutOfSpace.h"
#include "OutOfSpace/Component/StaminaComponent.h"


AOsPlayerCharacter::AOsPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 600.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

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
		HealthComp->MaxHealth = 500.f;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComp is null in AOsPlayerCharacter::AOsPlayerCharacter"));
	}

	StaminaComp = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComp"));

	Faction = EFaction::F_PLAYER;
}

void AOsPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	CurrentController = NewController;

	AOsPlayerController* osPController = Cast<AOsPlayerController>(NewController);
	if (osPController)
	{
		osPController->OnFire.AddUniqueDynamic(this, &AOsPlayerCharacter::Fire);
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
		TakeDamage(HealthComp->MaxHealth, FDamageEvent(), GetController(), this);
	}
}

void AOsPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
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
