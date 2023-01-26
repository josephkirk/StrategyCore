#include "LyraCameraModeTriggerMapping.h"
#include "LyraCameraModeTriggerMapping.h"
#include "LyraCameraModeTriggerMapping.h"
#include "Kismet/KismetMathLibrary.h"

TSubclassOf<ULyraCameraMode> FLyraCameraModesMapping::GetCameraModeFromValue(float TriggeredValue) const
{
	for (FLyraCameraModeValueMapping CameraModeTriggerMapping : CameraModesMapping) {
		if (CameraModeTriggerMapping.TriggeredValue == TriggeredValue) {
			return CameraModeTriggerMapping.CameraMode;
		}
	}
	return TSubclassOf<ULyraCameraMode>();
}

TSubclassOf<ULyraCameraMode> FLyraCameraModesMapping::NextCameraMode()
{
	if (CameraModesMapping.IsEmpty()) {
		return TSubclassOf<ULyraCameraMode>();
	}
	CurrentActiveMode = FMath::Min(CurrentActiveMode + 1, CameraModesMapping.Num() - 1);
	return CameraModesMapping[CurrentActiveMode].CameraMode;
}

TSubclassOf<ULyraCameraMode> FLyraCameraModesMapping::LowerCameraMode()
{
	if (CameraModesMapping.IsEmpty()) {
		return TSubclassOf<ULyraCameraMode>();
	}
	CurrentActiveMode = FMath::Max(CurrentActiveMode - 1, 0);
	return CameraModesMapping[CurrentActiveMode].CameraMode;
}

TSubclassOf<ULyraCameraMode> FLyraCameraModesMapping::CurrentCameraMode()
{
	if (CameraModesMapping.IsValidIndex(CurrentActiveMode)) {
		return CameraModesMapping[CurrentActiveMode].CameraMode;
	}
	return TSubclassOf<ULyraCameraMode>();
}

void FLyraCameraModesMapping::SetActiveCameraMode(const TSubclassOf<ULyraCameraMode> NewCameraMode)
{
	Sort();
	CurrentActiveMode = CameraModesMapping.IndexOfByPredicate([&](const FLyraCameraModeValueMapping LyraCameraModeValueMapping) {
		return LyraCameraModeValueMapping.CameraMode == NewCameraMode;
	});
}

void FLyraCameraModesMapping::Sort()
{
	if (!bIsSorted)
	{
		CameraModesMapping.Sort([](const FLyraCameraModeValueMapping A, const FLyraCameraModeValueMapping B) {
			return A.TriggeredValue < B.TriggeredValue;
		});
	}
	bIsSorted = true;
}

TSubclassOf<ULyraCameraMode> FLyraCameraModesMapping::GetMinValueTriggerCameraMode()
{
	if (CameraModesMapping.IsEmpty()) {
		return TSubclassOf<ULyraCameraMode>();
	}
	//Sort();
	return CameraModesMapping[0].CameraMode;
}

TSubclassOf<ULyraCameraMode> FLyraCameraModesMapping::GetMaxValueTriggerCameraMode()
{
	if (CameraModesMapping.IsEmpty()) {
		return TSubclassOf<ULyraCameraMode>();
	}
	//Sort();
	return CameraModesMapping.Last().CameraMode;
}
