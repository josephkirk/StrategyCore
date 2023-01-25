// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LyraCharacterMovementComponent.h"
#include "StrategyCoreUnitMovementComponent.generated.h"

/**
 * 
 */
UCLASS(Config = Game)
class STRATEGYCOREMODULERUNTIME_API UStrategyCoreUnitMovementComponent : public ULyraCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UStrategyCoreUnitMovementComponent(const FObjectInitializer& ObjectInitializer);
};
