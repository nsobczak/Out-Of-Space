#include "OutOfSpace/Player/OsPlayerController.h"

#include "OsCharacter.h"

AOsPlayerController::AOsPlayerController(const FObjectInitializer& ObjInitializer) : Super(
	ObjInitializer)
{
}

void AOsPlayerController::BeginPlay()
{
	Super::BeginPlay();


	// WorldSettings = 
	// GameInstance 
}

void AOsPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AOsPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AOsPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Log, TEXT("possessed pawn = %s"), InPawn ? *InPawn->GetName() : TEXT("null"));

	OsPawn = Cast<AOsCharacter>(GetPawn());
}

// void AOsPlayerController::OnUnPossess()
// {
// 	Super::OnUnPossess();
// }

void AOsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &AOsPlayerController::MoveForward).bExecuteWhenPaused = true;
	InputComponent->BindAxis("MoveRight", this, &AOsPlayerController::MoveRight).bExecuteWhenPaused = true;

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &AOsPlayerController::AddYawInput);
	InputComponent->BindAxis("TurnRate", this, &AOsPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &AOsPlayerController::AddPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AOsPlayerController::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(IE_Pressed, this, &AOsPlayerController::TouchStarted);
	InputComponent->BindTouch(IE_Released, this, &AOsPlayerController::TouchStopped);

	// Actions
	InputComponent->BindAction("Start", IE_Released, this, &AOsPlayerController::Start).bExecuteWhenPaused = true;
	InputComponent->BindAction("Accept", IE_Released, this, &AOsPlayerController::Accept).bExecuteWhenPaused = true;
	InputComponent->BindAction("Back", IE_Released, this, &AOsPlayerController::Back).bExecuteWhenPaused = true;

	InputComponent->BindAction("RollLeft", IE_Released, this, &AOsPlayerController::RollLeft);
	InputComponent->BindAction("RollRight", IE_Pressed, this, &AOsPlayerController::RollRight);
	InputComponent->BindAction("Lock", IE_Pressed, this, &AOsPlayerController::Lock);
	InputComponent->BindAction("Fire", IE_Released, this, &AOsPlayerController::Fire);
}


void AOsPlayerController::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	UE_LOG(LogInput, Log, TEXT("TouchStarted"));
}

void AOsPlayerController::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	UE_LOG(LogInput, Log, TEXT("TouchStopped"));
}

void AOsPlayerController::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	float Val = Rate * (OsPawn ? OsPawn->BaseTurnRate : DefaultBaseRate) * GetWorld()->GetDeltaSeconds();
	if (Val != 0.f && IsLocalPlayerController())
	{
		AddYawInput(Val);
	}
}

void AOsPlayerController::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information - pitch
	float Val = Rate * (OsPawn ? OsPawn->BaseLookUpRate : DefaultBaseRate) * GetWorld()->GetDeltaSeconds();
	if (Val != 0.f && IsLocalPlayerController())
	{
		AddPitchInput(Val);
	}
}

void AOsPlayerController::MoveForward(float Value)
{
	if (!IsPaused() && Value != 0.0f)
	{
		// find out which way is forward
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		if (GetPawn())
		{
			GetPawn()->AddMovementInput(Direction, Value);
		}
	}
}

void AOsPlayerController::MoveRight(float Value)
{
	if (!IsPaused() && Value != 0.0f)
	{
		// find out which way is right
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		if (GetPawn())
		{
			GetPawn()->AddMovementInput(Direction, Value);
		}
	}
}

// actions
void AOsPlayerController::Start()
{
	UE_LOG(LogInput, Log, TEXT("Start pressed"));

	Pause();
}

void AOsPlayerController::Accept()
{
	UE_LOG(LogInput, Log, TEXT("Accept pressed"));
}

void AOsPlayerController::Back()
{
	UE_LOG(LogInput, Log, TEXT("Back pressed"));
}

void AOsPlayerController::RollLeft()
{
	UE_LOG(LogInput, Log, TEXT("RollLeft pressed"));
}

void AOsPlayerController::RollRight()
{
	UE_LOG(LogInput, Log, TEXT("RollRight pressed"));
}

void AOsPlayerController::Lock()
{
	UE_LOG(LogInput, Log, TEXT("Lock pressed"));
}

void AOsPlayerController::Fire()
{
	UE_LOG(LogInput, Log, TEXT("Fire pressed"));

	// TODO: create event and bind in character
	if (OsPawn)
	{
		OsPawn->Fire();
	}
}
