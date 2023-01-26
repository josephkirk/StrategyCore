// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Camera/LyraCameraMode.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneComponent.h"
#include "LyraCameraMode_RTSCamera.generated.h"

class USceneComponent;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class STRATEGYCOREMODULERUNTIME_API ULyraCameraMode_RTSCamera : public ULyraCameraMode
{
	GENERATED_BODY()

public:

	ULyraCameraMode_RTSCamera();

protected:

	virtual void UpdateView(float DeltaTime) override;
	virtual void DrawDebug(UCanvas* Canvas) const override;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera")
		FRotator DefaultPivotRotation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera")
		float StartingHeight = 10000;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera")
		bool EnableCameraLag = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera")
		bool EnableCameraRotationLag = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Dynamic Camera Height Settings")
		bool EnableDynamicCameraHeight = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Dynamic Camera Height Settings")
		TEnumAsByte<ECollisionChannel> CollisionChannel;
	UPROPERTY(
		BlueprintReadWrite,
		EditAnywhere,
		Category = "RTSCamera - Dynamic Camera Height Settings",
		meta = (EditCondition = "EnableDynamicCameraHeight")
	)
		float FindGroundTraceLength = 100000;


private:

	void ConditionallyKeepCameraAtDesiredZoomAboveGround();
	void ConditionallyApplyCameraBounds() const;

	USceneComponent* GetTargetRootComponent() const;

	UPROPERTY()
		FName CameraBlockingVolumeTag;
	UPROPERTY()
		AActor* CameraFollowTarget;
	UPROPERTY()
		float DeltaSeconds;
	UPROPERTY()
		bool IsCameraOutOfBoundsErrorAlreadyDisplayed;
	UPROPERTY()
		FVector2D DragStartLocation;
};
