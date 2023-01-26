// Fill out your copyright notice in the Description page of Project Settings.


#include "StrategyCorePlayerHeroComponent.h"
#include "LyraLogChannels.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Player/LyraPlayerController.h"
#include "Player/LyraPlayerState.h"
#include "Player/LyraLocalPlayer.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "Character/LyraPawnData.h"
#include "Character/LyraCharacter.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Input/LyraInputConfig.h"
#include "Camera/LyraCameraComponent.h"
#include "LyraGameplayTags.h"
#include "StrategyCoreGameplayTags.h"
#include "Engine/LocalPlayer.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Settings/LyraSettingsLocal.h"
#include "System/LyraAssetManager.h"
#include "PlayerMappableInputConfig.h"
#include "Input/LyraMappableConfigPair.h"
#include "Camera/LyraCameraMode.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StrategyCorePlayerHeroComponent)

#define TEST_INPUTBIND

#ifdef TEST_INPUTBIND
	#define DEBUG_INPUTBIND true
#else
	#define DEBUG_INPUTBIND false
#endif

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Direction_Left, "Direction.Left")
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Direction_Right, "Direction.Right")
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Direction_Up, "Direction.Up")
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Direction_Down, "Direction.Down")
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Direction_In, "Direction.In")
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Direction_Out, "Direction.Out")
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Camera_Move, "StrategyCoreGame.Camera.State.Moving")
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Camera_Drag, "StrategyCoreGame.Camera.State.Dragging")
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Camera_EdgeScroll, "StrategyCoreGame.Camera.State.EdgeScrolling")
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Camera_Rotate, "StrategyCoreGame.Camera.State.Rotating")
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Camera_Zoom, "StrategyCoreGame.Camera.State.Zooming")

UStrategyCorePlayerHeroComponent::UStrategyCorePlayerHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityCameraMode = nullptr;
	bReadyToBindInputs = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = false;
}

void UStrategyCorePlayerHeroComponent::FollowTarget(AActor* Target)
{
}

void UStrategyCorePlayerHeroComponent::UnFollowTarget()
{
}

ALyraCharacter* UStrategyCorePlayerHeroComponent::GetLyraCharacter()
{
	return GetPawnChecked<ALyraCharacter>();
}

void UStrategyCorePlayerHeroComponent::OnRegister()
{
	Super::OnRegister();
}

void UStrategyCorePlayerHeroComponent::BeginPlay()
{
	Super::BeginPlay();
	ZoomMappings.SetActiveCameraMode ( DetermineCameraMode() ); 
	ZoomMappings = ZoomMappingsData->CameraModeTriggerMapping;
}

void UStrategyCorePlayerHeroComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ApplyMoveCameraCommands();
	ConditionallyPerformEdgeScrolling();
	DeltaSeconds = DeltaTime;
}


void UStrategyCorePlayerHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UStrategyCorePlayerHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULyraLocalPlayer* LP = Cast<ULyraLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	//ConditionallyEnableEdgeScrolling();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const ULyraPawnData* PawnData = PawnExtComp->GetPawnData<ULyraPawnData>())
		{
			if (const ULyraInputConfig* InputConfig = PawnData->InputConfig)
			{
			/*	const FStrategyCoreGameplayTags& GameplayTags = FStrategyCoreGameplayTags::Get();*/

				// Register any default input configs with the settings so that they will be applied to the player during AddInputMappings
				for (const FMappableConfigPair& Pair : DefaultInputConfigs)
				{
					{
						FModifyContextOptions Options = {};
						Options.bIgnoreAllPressedKeysUntilRelease = false;
						// Actually add the config to the local player							
						Subsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
					}
				}

				// Need to change expose LyInputConfig and LyraInputComponent
				ULyraInputComponent* LyraIC = CastChecked<ULyraInputComponent>(PlayerInputComponent);
				LyraIC->AddInputMappings(InputConfig, Subsystem);

				TArray<uint32> BindHandles;
				LyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

				BindInputTags(LyraIC, InputConfig);

			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UStrategyCorePlayerHeroComponent::Input_MoveCamera(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	// If the player has attempted to move again then cancel auto running
	if (ALyraPlayerController* LyraController = Cast<ALyraPlayerController>(Controller))
	{
		LyraController->SetIsAutoRunning(false);
	}

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		FGameplayTagContainer CameraTags;

		if (Value.X != 0.0f)
		{
			if (Value.X > 0.0f) {
				CameraTags.AddTag(TAG_Direction_Right);
			}
			else {
				CameraTags.AddTag(TAG_Direction_Left);
			}
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			//Pawn->AddMovementInput(MovementDirection, Value.X);
			QueueCameraMovementCommand(MovementDirection, Value.X, CameraTags);
		}

		if (Value.Y != 0.0f)
		{
			if (Value.Y > 0.0f) {
				CameraTags.AddTag(TAG_Direction_Up);
			}
			else {
				CameraTags.AddTag(TAG_Direction_Down);
			}
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			//Pawn->AddMovementInput(MovementDirection, Value.Y);
			QueueCameraMovementCommand(MovementDirection, Value.Y, CameraTags);
		}
	}
}

void UStrategyCorePlayerHeroComponent::Input_DragCamera(const FInputActionValue& InputActionValue)
{
	if (!IsDragging && InputActionValue.Get<bool>())
	{
		IsDragging = true;
		DragStartLocation = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	}

	else if (IsDragging && InputActionValue.Get<bool>())
	{
		APawn* Pawn = GetPawn<APawn>();
		AController* Controller = Pawn ? Pawn->GetController() : nullptr;

		// If the player has attempted to move again then cancel auto running
		if (ALyraPlayerController* LyraController = Cast<ALyraPlayerController>(Controller))
		{
			LyraController->SetIsAutoRunning(false);
		}

		const auto MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
		auto DragExtents = UWidgetLayoutLibrary::GetViewportWidgetGeometry(GetWorld()).GetLocalSize();
		DragExtents *= DragExtent;

		auto Delta = MousePosition - DragStartLocation;
		Delta.X = FMath::Clamp(Delta.X, -DragExtents.X, DragExtents.X) / DragExtents.X;
		Delta.Y = FMath::Clamp(Delta.Y, -DragExtents.Y, DragExtents.Y) / DragExtents.Y;

		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		FGameplayTagContainer CameraTags;
		CameraTags.AddTagFast(TAG_Camera_Drag);
		if (Delta.X > 0.0f) {
			CameraTags.AddTag(TAG_Direction_Right);
		}
		else {
			CameraTags.AddTag(TAG_Direction_Left);
		}
		if (Delta.Y > 0.0f) {
			CameraTags.AddTag(TAG_Direction_Up);
		}
		else {
			CameraTags.AddTag(TAG_Direction_Down);
		}
		QueueCameraMovementCommand(
			MovementRotation.RotateVector(FVector::RightVector),
			Delta.X, CameraTags);

		QueueCameraMovementCommand(
			MovementRotation.RotateVector(FVector::ForwardVector),
			Delta.Y * -1, CameraTags);

	}

	else if (IsDragging && !InputActionValue.Get<bool>())
	{
		IsDragging = false;
	}
}

void UStrategyCorePlayerHeroComponent::Input_ZoomCamera(const FInputActionValue& InputActionValue)
{
	const float Value = InputActionValue.Get<float>();
	AbilityCameraMode = nullptr;
	if (Value > 0.0f)
	{
		ZoomMappings.LowerCameraMode();
	} else {
		ZoomMappings.NextCameraMode();
	}
	if (ZoomMappings.CurrentCameraMode() != nullptr && AbilityCameraMode == nullptr) {
		AbilityCameraMode = ZoomMappings.CurrentCameraMode();
	}
}

void UStrategyCorePlayerHeroComponent::Input_EdgeScrollCamera(const FInputActionValue& InputActionValue)
{
	UE_LOG(LogStrategyCore, Display, TEXT("StrategyCore EdgeScroll Input. [%s]"), *InputActionValue.ToString());
	//ConditionallyPerformEdgeScrolling();
}

void UStrategyCorePlayerHeroComponent::Input_RotateCameraLeft(const FInputActionValue& InputActionValue)
{
	/*APawn* Pawn = GetPawn<APawn>();
	Pawn->AddControllerYawInput(-90.0f);
	Pawn->SetActorRelativeRotation(
		FRotator::MakeFromEuler(
			FVector(
				0.0f,
				0.0,
				-90.0f
			)
		)
	);*/
	FGameplayTagContainer CameraTag;
	CameraTag.AddTag(TAG_Direction_Left);
	QueueCameraRotationCommand(-90.0f, CameraTag);
}

void UStrategyCorePlayerHeroComponent::Input_RotateCameraRight(const FInputActionValue& InputActionValue)
{

	//APawn* Pawn = GetPawn<APawn>();
	//Pawn->AddControllerYawInput(90.0f);
	//Pawn->SetActorRelativeRotation(
	//	FRotator::MakeFromEuler(
	//		FVector(
	//			0,
	//			0,
	//			90
	//		)
	//	)
	//);

	FGameplayTagContainer CameraTag;
	CameraTag.AddTag(TAG_Direction_Right);
	QueueCameraRotationCommand(90.0f, CameraTag);
}

void UStrategyCorePlayerHeroComponent::QueueCameraMovementCommand(const FVector Direction, const double Scale, FGameplayTagContainer CameraMovementTag)
{
	CameraMovementTag.AddTagFast(TAG_Camera_Move);
	FCameraMovementCommand MovementCommand;
	MovementCommand.Direction = Direction;
	MovementCommand.Scale = Scale;
	MovementCommand.CameraTag = CameraMovementTag;
	CameraMovementCommandQueue.Enqueue(MovementCommand);
}

void UStrategyCorePlayerHeroComponent::QueueCameraRotationCommand(const float Rotation, FGameplayTagContainer CameraMovementTag)
{
	CameraMovementTag.AddTagFast(TAG_Camera_Rotate);
	FCameraMovementCommand MovementCommand;
	MovementCommand.Rotation = Rotation;
	MovementCommand.CameraTag = CameraMovementTag;
	CameraMovementCommandQueue.Enqueue(MovementCommand);
}

void UStrategyCorePlayerHeroComponent::ApplyMoveCameraCommands()
{
	APawn* Pawn = GetPawn<APawn>();
	while(!CameraMovementCommandQueue.IsEmpty())
	{
		FCameraMovementCommand MovementCommand;
		CameraMovementCommandQueue.Dequeue(MovementCommand);
		Pawn->AddMovementInput(
			MovementCommand.Direction, MovementCommand.Scale
		);
		if (MovementCommand.Rotation != 0.0) {
			Pawn->AddControllerYawInput(MovementCommand.Rotation);
		}
		UE_LOG(LogStrategyCore, Display, TEXT("StrategyCore Camera Movement. [%s]"), *MovementCommand.CameraTag.ToStringSimple());
	}
}

void UStrategyCorePlayerHeroComponent::ConditionallyEnableEdgeScrolling() const
{
	if (EnableEdgeScrolling)
	{
		//FInputModeGameAndUI InputMode;
		//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		// InputMode.SetHideCursorDuringCapture(false);
		//APlayerController* PC = GetController<APlayerController>();
		//check(PC);
		//PC->SetInputMode(InputMode);
		//PC->bShowMouseCursor = true;
	}
}

/// <summary>
/// TODO:
/// Find way to better implement edgescrolling
/// </summary>
void UStrategyCorePlayerHeroComponent::ConditionallyPerformEdgeScrolling()
{
	if (EnableEdgeScrolling && !IsDragging)
	{
		
		EdgeScrollLeft();
		EdgeScrollRight();
		EdgeScrollUp();
		EdgeScrollDown();
	}
}

void UStrategyCorePlayerHeroComponent::EdgeScrollLeft()
{
	APawn* Pawn = GetPawn<APawn>();
	//UE_LOG(LogStrategyCore, Display, TEXT("StrategyCore EdgeScroll Camera Left."));
	const auto MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	const auto ViewpoStrategyCoreize = UWidgetLayoutLibrary::GetViewportWidgetGeometry(GetWorld()).GetLocalSize();
	const auto NormalizedMousePosition = 1 - UKismetMathLibrary::NormalizeToRange(
		MousePosition.X,
		0.0f,
		ViewpoStrategyCoreize.X * 0.05f
	);

	const auto Movement = UKismetMathLibrary::FClamp(NormalizedMousePosition, 0.0, 1.0);
	const auto Direction = GetOwner()->GetActorRightVector();
	if (Movement > 0.0f) {
		// UE_LOG(LogStrategyCore, Display, TEXT("StrategyCore EdgeScroll Camera Left. [%d]"), Movement);
		// Pawn->AddMovementInput(
		// 	-1 * Direction, Movement * EdgeScrollSpeed
		// );
		FGameplayTagContainer CameraTags;
		CameraTags.AddTag(TAG_Camera_EdgeScroll);
		CameraTags.AddTag(TAG_Direction_Left);
		QueueCameraMovementCommand(-1 * Direction, Movement * EdgeScrollSpeed, CameraTags);
	}
}

void UStrategyCorePlayerHeroComponent::EdgeScrollRight()
{
	APawn* Pawn = GetPawn<APawn>();
	const auto MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	const auto ViewpoStrategyCoreize = UWidgetLayoutLibrary::GetViewportWidgetGeometry(GetWorld()).GetLocalSize();
	const auto NormalizedMousePosition = UKismetMathLibrary::NormalizeToRange(
		MousePosition.X,
		ViewpoStrategyCoreize.X * 0.95f,
		ViewpoStrategyCoreize.X
	);

	const auto Movement = UKismetMathLibrary::FClamp(NormalizedMousePosition, 0.0, 1.0);
	const auto Direction = GetOwner()->GetActorRightVector();
	
	if (Movement > 0.0f) {
		// UE_LOG(LogStrategyCore, Display, TEXT("StrategyCore EdgeScroll Camera Right. [%d]"), Movement);
		// Pawn->AddMovementInput(
		// 	Direction, Movement * EdgeScrollSpeed
		// );
		FGameplayTagContainer CameraTags;
		CameraTags.AddTag(TAG_Camera_EdgeScroll);
		CameraTags.AddTag(TAG_Direction_Right);
		QueueCameraMovementCommand(Direction, Movement * EdgeScrollSpeed, CameraTags);
	}
}

void UStrategyCorePlayerHeroComponent::EdgeScrollUp()
{
	APawn* Pawn = GetPawn<APawn>();
	const auto MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	const auto ViewpoStrategyCoreize = UWidgetLayoutLibrary::GetViewportWidgetGeometry(GetWorld()).GetLocalSize();
	const auto NormalizedMousePosition = UKismetMathLibrary::NormalizeToRange(
		MousePosition.Y,
		0.0f,
		ViewpoStrategyCoreize.Y * 0.05f
	);

	const auto Movement = 1 - UKismetMathLibrary::FClamp(NormalizedMousePosition, 0.0, 1.0);
	const auto Direction = GetOwner()->GetActorForwardVector();

	if (Movement > 0.0f) {
		// UE_LOG(LogStrategyCore, Display, TEXT("StrategyCore EdgeScroll Camera Right. [%d]"), Movement);
		// Pawn->AddMovementInput(
		// 	Direction, Movement * EdgeScrollSpeed
		// );
		FGameplayTagContainer CameraTags;
		CameraTags.AddTag(TAG_Camera_EdgeScroll);
		CameraTags.AddTag(TAG_Direction_Up);
		QueueCameraMovementCommand(Direction, Movement * EdgeScrollSpeed, CameraTags);
	}
}

void UStrategyCorePlayerHeroComponent::EdgeScrollDown()
{
	APawn* Pawn = GetPawn<APawn>();
	const auto MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	const auto ViewpoStrategyCoreize = UWidgetLayoutLibrary::GetViewportWidgetGeometry(GetWorld()).GetLocalSize();
	const auto NormalizedMousePosition = UKismetMathLibrary::NormalizeToRange(
		MousePosition.Y,
		ViewpoStrategyCoreize.Y * 0.95f,
		ViewpoStrategyCoreize.Y
	);

	const auto Movement = UKismetMathLibrary::FClamp(NormalizedMousePosition, 0.0, 1.0);
	const auto Direction = GetOwner()->GetActorForwardVector();

	if (Movement > 0.0f) {
		// UE_LOG(LogStrategyCore, Display, TEXT("StrategyCore EdgeScroll Camera Right. [%d]"), Movement);
		// Pawn->AddMovementInput(
		// 	-1 * Direction, Movement * EdgeScrollSpeed
		// );
		FGameplayTagContainer CameraTags;
		CameraTags.AddTag(TAG_Camera_EdgeScroll);
		CameraTags.AddTag(TAG_Direction_Down);
		QueueCameraMovementCommand(-1*Direction, Movement * EdgeScrollSpeed, CameraTags);
	};
}

void UStrategyCorePlayerHeroComponent::BindInputTags(ULyraInputComponent* PlayerInputComponent, const ULyraInputConfig* InputConfig)
{
	PlayerInputComponent->BindNativeAction(InputConfig, Input_CameraMove, ETriggerEvent::Triggered, this, &ThisClass::Input_MoveCamera, /*bLogIfNotFound=*/ DEBUG_INPUTBIND);
	PlayerInputComponent->BindNativeAction(InputConfig, Input_CameraTurnLeft, ETriggerEvent::Triggered, this, &ThisClass::Input_RotateCameraLeft, /*bLogIfNotFound=*/ DEBUG_INPUTBIND);
	PlayerInputComponent->BindNativeAction(InputConfig, Input_CameraTurnRight, ETriggerEvent::Triggered, this, &ThisClass::Input_RotateCameraRight, /*bLogIfNotFound=*/ DEBUG_INPUTBIND);
	PlayerInputComponent->BindNativeAction(InputConfig, InputTag_Camera_EdgeScroll, ETriggerEvent::Ongoing, this, &ThisClass::Input_EdgeScrollCamera, /*bLogIfNotFound=*/ DEBUG_INPUTBIND);
	PlayerInputComponent->BindNativeAction(InputConfig, Input_CameraDrag, ETriggerEvent::Triggered, this, &ThisClass::Input_DragCamera, /*bLogIfNotFound=*/ DEBUG_INPUTBIND);
	PlayerInputComponent->BindNativeAction(InputConfig, Input_CameraZoom, ETriggerEvent::Triggered, this, &ThisClass::Input_ZoomCamera, /*bLogIfNotFound=*/ DEBUG_INPUTBIND);
}
