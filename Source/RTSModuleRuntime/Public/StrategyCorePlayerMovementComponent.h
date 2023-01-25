// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LyraCharacterMovementComponent.h"
#include "StrategyCorePlayerMovementComponent.generated.h"

/**
 * 
 */
UCLASS(Config = Game)
class STRATEGYCOREMODULERUNTIME_API UStrategyCorePlayerMovementComponent : public ULyraCharacterMovementComponent
{
	GENERATED_BODY()
public:

	UStrategyCorePlayerMovementComponent(const FObjectInitializer& ObjectInitializer);

};
