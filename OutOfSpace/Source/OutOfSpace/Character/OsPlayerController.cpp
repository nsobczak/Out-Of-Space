#include "OutOfSpace/Character/OsPlayerController.h"

#include "OsCharacter.h"
#include "OsPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "OutOfSpace/OutOfSpace.h"
#include "OutOfSpace/Game/OsGameMode.h"
#include "OutOfSpace/Game/OsWorldSettings.h"

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

	OsWorldSettings = Cast<AOsWorldSettings>(GetWorldSettings());
}

bool AOsPlayerController::UpdateCrosshairScreenLocation(FVector2D& screenLocation) const
{
	int32 ScreenWidth = 0, ScreenHeight = 0;
	GetViewportSize(ScreenWidth, ScreenHeight);

	if (ProjectWorldLocationToScreen(OsPlayerCharacter->GetCrosshairWorldLocation(), screenLocation, false))
	{
		// UE_LOG(LogOoS, Log, TEXT("crosshair screenLocation = %s | viewport size = (%d, %d)"),
		// 	*screenLocation.ToString(), ScreenWidth, ScreenHeight);
	}

	int32 ScreenX = (int32)screenLocation.X;
	int32 ScreenY = (int32)screenLocation.Y;

	return ScreenX >= 0 && ScreenY >= 0 && ScreenX < ScreenWidth && ScreenY < ScreenHeight;
}

void AOsPlayerController::UpdateCrosshair()
{
	CrosshairScreenLocation = UpdateCrosshairScreenLocation(CrosshairScreenLocation) ? CrosshairScreenLocation :
	                          FVector2D::ZeroVector;

	// TODO: AOsPlayerCharacter* osPlayerChar make param + update onPossess
	AOsPlayerCharacter* osPlayerChar = Cast<AOsPlayerCharacter>(OsPlayerCharacter);
	if (!osPlayerChar) { return; }

	// TODO: change ECollisionChannel::ECC_Pawn when needed
	EObjectTypeQuery objectTypeQuery = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes = TArray<TEnumAsByte<EObjectTypeQuery>>{objectTypeQuery};
	TArray<AActor*> actorToIgnore = TArray<AActor*>{osPlayerChar};
	TArray<FHitResult> outHits;
	UKismetSystemLibrary::SphereTraceMultiForObjects(this, osPlayerChar->GetFollowCamera()->GetComponentLocation(),
		osPlayerChar->GetFollowCamera()->GetComponentLocation() + SphereTraceLengthDetection * osPlayerChar->
		GetFollowCamera()->GetForwardVector(), SphereTraceSphereRadius, objectTypes, false, actorToIgnore,
		EDrawDebugTrace::ForOneFrame, outHits, true, FLinearColor::Blue, FLinearColor::Red, 1.f);

	// DrawDebugLine(GetWorld(), osPlayerChar->GetFollowCamera()->GetComponentLocation(),
	// 	osPlayerChar->GetFollowCamera()->GetComponentLocation() + SphereTraceLengthDetection * osPlayerChar->GetFollowCamera()
	// 	->GetForwardVector(), FColor::Green, false, 0, 0, 3.f);

	if (outHits.Num() <= 0 && bIsCrosshairLocked)
	{
		bIsCrosshairLocked = false;
		OnCrosshairLock.Broadcast(bIsCrosshairLocked);
	}
	else
	{
		for (int i = 0; i < outHits.Num(); ++i)
		{
			AOsCharacter* hitOsChar = Cast<AOsCharacter>(outHits[i].GetActor());
			if (hitOsChar)
			{
				// Get crosshair screen position and compare dist to decide if we should lock
				FVector2D characterScreenLocation;
				if (ProjectWorldLocationToScreen(hitOsChar->GetActorLocation(), characterScreenLocation, true))
				{
					bool newCrosshairLockedState = FVector2D::DistSquared(CrosshairScreenLocation,
						characterScreenLocation) < FMath::Square(ScreenDistanceDetectionThreshold);

					if (newCrosshairLockedState != bIsCrosshairLocked)
					{
						// broadcast only if a change happened
						bIsCrosshairLocked = newCrosshairLockedState;
						OnCrosshairLock.Broadcast(bIsCrosshairLocked);
					}
				}
			}
		}
	}
}

void AOsPlayerController::HandleMoveForward()
{
	// bool bMoveForward = false;
	TArray<FName> ActionsNames;
	GetDefault<UInputSettings>()->GetActionNames(ActionsNames);

	UInputSettings* inputSettings = GetMutableDefault<UInputSettings>();
	for (FName inputName : ActionsNames)
	{
		TArray<FInputActionKeyMapping> actionKeys;
		inputSettings->GetActionMappingByName(inputName, actionKeys);

		if (inputName.IsEqual(Action_MoveForward))
		{
			for (FInputActionKeyMapping key : actionKeys)
			{
				if (IsInputKeyDown(key.Key))
				{
					// bMoveForward = true;
					MoveForward();
					return;
				}
			}
		}
	}
}

void AOsPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateCrosshair();

	HandleMoveForward();
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

	OsPlayerCharacter = Cast<AOsPlayerCharacter>(GetPawn());
}

// void AOsPlayerController::OnUnPossess()
// {
// 	Super::OnUnPossess();
// }

void AOsPlayerController::Pause()
{
	if (OsGameMode && OsGameMode->bIsGamePlaying)
	{
		Super::Pause();
		OnPause.Broadcast(IsPaused());
	}
}

void AOsPlayerController::HandleIsGamePlayingUpdated(bool newVal)
{
	bArePlayerActionsAllowed = newVal;
}

float AOsPlayerController::GetGoalCompletion() const
{
	return (float)OsPlayerCharacter->GetFoeKilledCount() / (float)OsWorldSettings->TargetFoeAmount;
}

#pragma region Inputs
//______________________________________________________________

void AOsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up gameplay key bindings
	check(InputComponent);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis(Axis_Turn, this, &AOsPlayerController::AddYawInput);
	InputComponent->BindAxis(Axis_TurnRate, this, &AOsPlayerController::TurnAtRate);
	InputComponent->BindAxis(Axis_LookUp, this, &AOsPlayerController::AddPitchInput);
	InputComponent->BindAxis(Axis_LookUpRate, this, &AOsPlayerController::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(IE_Pressed, this, &AOsPlayerController::TouchStarted);
	InputComponent->BindTouch(IE_Released, this, &AOsPlayerController::TouchStopped);

	// Actions
	InputComponent->BindAction(Action_MoveForward, IE_Pressed, this, &AOsPlayerController::MoveForward).
	                bExecuteWhenPaused = true;
	InputComponent->BindAction(Action_Start, IE_Released, this, &AOsPlayerController::Start).bExecuteWhenPaused = true;
	InputComponent->BindAction(Action_Accept, IE_Released, this, &AOsPlayerController::Accept).bExecuteWhenPaused =
		true;
	InputComponent->BindAction(Action_Back, IE_Released, this, &AOsPlayerController::Back).bExecuteWhenPaused = true;

	InputComponent->BindAction(Action_RollLeft, IE_Released, this, &AOsPlayerController::RollLeft);
	InputComponent->BindAction(Action_RollRight, IE_Pressed, this, &AOsPlayerController::RollRight);
	InputComponent->BindAction(Action_Fire, IE_Released, this, &AOsPlayerController::Fire);
	// InputComponent->BindAction(Action_Lock, IE_Pressed, this, &AOsPlayerController::LockStart);
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
		// calculate delta for this frame from the rate information
		float Val = Rate * (OsPlayerCharacter ? OsPlayerCharacter->BaseTurnRate : DefaultBaseRate) * GetWorld()->
			GetDeltaSeconds();
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
		// calculate delta for this frame from the rate information - pitch
		float Val = Rate * (OsPlayerCharacter ? OsPlayerCharacter->BaseLookUpRate : DefaultBaseRate) * GetWorld()->
			GetDeltaSeconds();
		if (Val != 0.f && IsLocalPlayerController())
		{
			AddPitchInput(Val);
		}
	}
}

// === actions ===

void AOsPlayerController::MoveForward()
{
	OnMoveForward.Broadcast();

	if (!IsPaused() && bArePlayerActionsAllowed)
	{
		// find out which way is forward
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		if (GetPawn())
		{
			GetPawn()->AddMovementInput(Direction, 1.f);
		}
	}
}

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

//____________________________________________________________________________
#pragma endregion
