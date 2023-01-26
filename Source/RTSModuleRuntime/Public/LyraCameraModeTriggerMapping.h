#pragma once

#include "Camera/LyraCameraMode.h"
#include "Containers/Array.h"
#include "Engine/DataAsset.h"
#include "Templates/SubclassOf.h"
#include "UObject/UObjectGlobals.h"
#include "LyraCameraModeTriggerMapping.generated.h"

USTRUCT(Blueprintable)
struct FLyraCameraModeValueMapping
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mapping")
		FName DisplayName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mapping")
		TSubclassOf<ULyraCameraMode> CameraMode;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mapping")
		float TriggeredValue;

};

USTRUCT(Blueprintable)
struct FLyraCameraModesMapping
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mappings")
		TArray<FLyraCameraModeValueMapping> CameraModesMapping;
	
	TSubclassOf<ULyraCameraMode> GetCameraModeFromValue(float TriggerValue) const;

	TSubclassOf<ULyraCameraMode> GetMinValueTriggerCameraMode();

	TSubclassOf<ULyraCameraMode> GetMaxValueTriggerCameraMode();

	TSubclassOf<ULyraCameraMode> NextCameraMode();

	TSubclassOf<ULyraCameraMode> LowerCameraMode();

	TSubclassOf<ULyraCameraMode> CurrentCameraMode();

	void SetActiveCameraMode(const TSubclassOf<ULyraCameraMode> NewCameraMode);
private:
	void Sort();
	bool bIsSorted = false;
	int CurrentActiveMode = INDEX_NONE;
	
};

UCLASS(Blueprintable , Const, Meta = (DisplayName = "Camera Modes Stack with trigger value", ShortTooltip = "Data asset used to define a Camera Mode with trigger value mapping."))
class STRATEGYCOREMODULERUNTIME_API ULyraCameraModesMappingData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Modes Mappings")
		FLyraCameraModesMapping CameraModeTriggerMapping;
};