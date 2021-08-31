#include "OutOfSpace/Character/OsPlayerController.h"

#include "OsCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "OutOfSpace/OutOfSpace.h"
#include "OutOfSpace/Game/OsGameMode.h"

AOsPlayerController::AOsPlayerController(const FObjectInitializer& ObjInitializer) : Super(
	ObjInitializer)
{
}

void AOsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	OsGameMode = Cast<AOsGameMode>(UGameplayStatics::GetGameMode(this));
	if (OsGameMode)
	{
		OsGameMode->OnIsGamePlayingUpdated.AddUniqueDynamic(this, &AOsPlayerController::HandleIsGamePlayingUpdated);
	}
}

void AOsPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AOsPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (OsGameMode)
	{
		OsGameMode->OnIsGamePlayingUpdated.RemoveDynamic(this, &AOsPlayerController::HandleIsGamePlayingUpdated);
	}
}

void AOsPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogOoS, Log, TEXT("possessed pawn = %s"), InPawn ? *InPawn->GetName() : TEXT("null"));

	OnNewPawnPossessed.Broadcast(InPawn);

	OsPawn = Cast<AOsCharacter>(GetPawn());
}

// void AOsPlayerController::OnUnPossess()
// {
// 	Super::OnUnPossess();
// }

void AOsPlayerController::HandleIsGamePlayingUpdated(bool newVal)
{
	bArePlayerActionsAllowed = newVal;
}

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
	InputComponent->BindAction("Fire", IE_Released, this, &AOsPlayerController::Fire);
	// InputComponent->BindAction("Lock", IE_Pressed, this, &AOsPlayerController::LockStart);
	// InputComponent->BindAction("Fire", IE_Released, this, &AOsPlayerController::LockStart);
}


void AOsPlayerController::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	UE_LOG(LogInput, Log, TEXT("TouchStarted"));
}

void AOsPlayerController::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	UE_LOG(LogInput, Log, TEXT("TouchStopped"));
}

void AOsPlayerController::AddYawInput(float Val)
{
	if (!IsPaused() && bArePlayerActionsAllowed)
	{
		Super::AddYawInput(Val);
	}
}

void AOsPlayerController::TurnAtRate(float Rate)
{
	if (!IsPaused() && bArePlayerActionsAllowed)
	{
		UE_LOG(LogTemp, Log, TEXT("TurnAtRate"));

		// calculate delta for this frame from the rate information
		float Val = Rate * (OsPawn ? OsPawn->BaseTurnRate : DefaultBaseRate) * GetWorld()->GetDeltaSeconds();
		if (Val != 0.f && IsLocalPlayerController())
		{
			AddYawInput(Val);
		}
	}
}

void AOsPlayerController::AddPitchInput(float Val)
{
	if (!IsPaused() && bArePlayerActionsAllowed)
	{
		Super::AddPitchInput(Val);
	}
}

void AOsPlayerController::LookUpAtRate(float Rate)
{
	if (!IsPaused() && bArePlayerActionsAllowed)
	{
		UE_LOG(LogTemp, Log, TEXT("LookUpAtRate"));

		// calculate delta for this frame from the rate information - pitch
		float Val = Rate * (OsPawn ? OsPawn->BaseLookUpRate : DefaultBaseRate) * GetWorld()->GetDeltaSeconds();
		if (Val != 0.f && IsLocalPlayerController())
		{
			AddPitchInput(Val);
		}
	}
}

void AOsPlayerController::MoveForward(float Value)
{
	OnMoveForward.Broadcast(Value);

	if (!IsPaused() && bArePlayerActionsAllowed && Value != 0.0f)
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
	OnMoveRight.Broadcast(Value);

	if (!IsPaused() && bArePlayerActionsAllowed && Value != 0.0f)
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

// === actions ===

void AOsPlayerController::Start()
{
	OnStart.Broadcast();

	Pause();
}

void AOsPlayerController::Accept()
{
	UE_LOG(LogInput, Log, TEXT("Accept pressed"));

	OnAccept.Broadcast();
}

void AOsPlayerController::Back()
{
	UE_LOG(LogInput, Log, TEXT("Back pressed"));

	OnBack.Broadcast();
}

void AOsPlayerController::RollLeft()
{
	UE_LOG(LogInput, Log, TEXT("RollLeft pressed"));

	if (!IsPaused() && bArePlayerActionsAllowed)
	{
		// TODO: implement roll
		OnRoll.Broadcast(false);
	}
}

void AOsPlayerController::RollRight()
{
	UE_LOG(LogInput, Log, TEXT("RollRight pressed"));

	if (!IsPaused() && bArePlayerActionsAllowed)
	{
		// TODO: implement roll
		OnRoll.Broadcast(true);
	}
}

void AOsPlayerController::LockStart()
{
	UE_LOG(LogInput, Log, TEXT("LockStart pressed"));

	if (!IsPaused() && bArePlayerActionsAllowed)
	{
		// TODO: implement LockStart
	}
}

void AOsPlayerController::LockCancel()
{
	UE_LOG(LogInput, Log, TEXT("LockCancel pressed"));

	if (!IsPaused() && bArePlayerActionsAllowed)
	{
		// TODO: implement LockCancel
	}
}

void AOsPlayerController::Fire()
{
	if (!IsPaused() && bArePlayerActionsAllowed)
	{
		OnFire.Broadcast();
	}
}
