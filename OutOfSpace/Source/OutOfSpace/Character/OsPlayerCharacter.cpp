// 


#include "OsPlayerCharacter.h"

#include "OsPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
// #include "OutOfSpace/Character/OsPlayerController.h"


// Sets default values
AOsPlayerCharacter::AOsPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
