// Copyright 2022, Jesus Bracho, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CameraBlockingVolume.h"
#include "StrategyCoreCameraBoundsVolume.generated.h"

UCLASS()
class STRATEGYCOREMODULERUNTIME_API AStrategyCoreCameraBoundsVolume : public ACameraBlockingVolume
{
	GENERATED_BODY()

	AStrategyCoreCameraBoundsVolume();
};
