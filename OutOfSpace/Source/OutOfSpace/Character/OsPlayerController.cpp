#include "OutOfSpace/Character/OsPlayerController.h"

#include "OsCharacter.h"
#include "OsPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "OutOfSpace/OutOfSpace.h"
#include "OutOfSpace/Component/LockControllerComponent.h"
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

bool AOsPlayerController::IsActionButtonHeldDown(const FName ActionNameRef)
{
	bool result = false;

	TArray<FName> actionsNames;
	GetDefault<UInputSettings>()->GetActionNames(actionsNames);

	UInputSettings* inputSettings = GetMutableDefault<UInputSettings>();
	for (FName inputName : actionsNames)
	{
		TArray<FInputActionKeyMapping> actionKeys;
		inputSettings->GetActionMappingByName(inputName, actionKeys);

		if (inputName.IsEqual(ActionNameRef))
		{
			for (FInputActionKeyMapping key : actionKeys)
			{
				if (IsInputKeyDown(key.Key))
				{
					result = true;
				}
			}
		}
	}

	return result;
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

void AOsPlayerController::HandleEnemyDetection()
{
	// check if button is down
	// TODO: add foe to tarray<aactor> targetFoes and currentLockCount++ and (reset timeRemainingBeforeAddingAgain or add to tmap)
	// if foe is close enough to crosshair and (time remaining in tmap <=0 or tmap does not contains) and currentLockCount < canLockCount
	// - tmap<aactor foe, float timeRemainingBeforeAddingAgain>
	// fire to each target on button released, and reset everything


	CrosshairScreenLocation = UpdateCrosshairScreenLocation(CrosshairScreenLocation) ? CrosshairScreenLocation :
	                          FVector2D::ZeroVector;

	if (!OsPlayerCharacter) { return; }

	// TODO: change ECollisionChannel::ECC_Pawn when needed
	EObjectTypeQuery objectTypeQuery = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes = TArray<TEnumAsByte<EObjectTypeQuery>>{objectTypeQuery};
	TArray<AActor*> actorToIgnore = TArray<AActor*>{OsPlayerCharacter};
	TArray<FHitResult> outHits;
	UKismetSystemLibrary::SphereTraceMultiForObjects(this, OsPlayerCharacter->GetFollowCamera()->GetComponentLocation(),
		OsPlayerCharacter->GetFollowCamera()->GetComponentLocation() + SphereTraceLengthDetection * OsPlayerCharacter->
		GetFollowCamera()->GetForwardVector(), SphereTraceSphereRadius, objectTypes, false, actorToIgnore,
		EDrawDebugTrace::None, outHits, true, FLinearColor::Blue, FLinearColor::Red, 1.f);

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
					// Crosshair
					bool newCrosshairLockedState = FVector2D::DistSquared(CrosshairScreenLocation,
						characterScreenLocation) < FMath::Square(LockCrosshairScreenDistanceThreshold);

					if (newCrosshairLockedState != bIsCrosshairLocked)
					{
						// broadcast only if a change happened
						bIsCrosshairLocked = newCrosshairLockedState;
						OnCrosshairLock.Broadcast(bIsCrosshairLocked);
					}

					// Fire lock
					if (IsActionButtonHeldDown(Action_Fire))
					{
						bool newLockedFireState = FVector2D::DistSquared(CrosshairScreenLocation,
							characterScreenLocation) < FMath::Square(LockFireScreenDistanceThreshold);
						if (OsPlayerCharacter->GetLockController())
						{
							// TODO: handle adding same actor several times in lock foes array -
							// - sort function to put what actor should be added in 1st position
							// - timer to delay enemy addition to array in lock controller
							OsPlayerCharacter->GetLockController()->LockCharacter(hitOsChar);
						}
					}
				}
			}
		}
	}
}

void AOsPlayerController::HandleMoveForward()
{
	// bool bMoveForward = false;

	if (IsActionButtonHeldDown(Action_MoveForward))
	{
		MoveForward();
		return;
	}
}

void AOsPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	HandleEnemyDetection();

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
	// InputComponent->BindAction(Action_MoveForward, IE_Pressed, this, &AOsPlayerController::MoveForward).
	//                 bExecuteWhenPaused = true;
	InputComponent->BindAction(Action_Start, IE_Released, this, &AOsPlayerController::Start).bExecuteWhenPaused = true;
	InputComponent->BindAction(Action_Accept, IE_Released, this, &AOsPlayerController::Accept).bExecuteWhenPaused =
		true;
	InputComponent->BindAction(Action_Back, IE_Released, this, &AOsPlayerController::Back).bExecuteWhenPaused = true;

	InputComponent->BindAction(Action_RollLeft, IE_Pressed, this, &AOsPlayerController::RollLeft);
	InputComponent->BindAction(Action_RollRight, IE_Pressed, this, &AOsPlayerController::RollRight);
	InputComponent->BindAction(Action_Fire, IE_Released, this, &AOsPlayerController::Fire);
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
	if (!IsPaused() && bArePlayerActionsAllowed)
	{
		OnRoll.Broadcast(false);
	}
}

void AOsPlayerController::RollRight()
{
	if (!IsPaused() && bArePlayerActionsAllowed)
	{
		OnRoll.Broadcast(true);
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
