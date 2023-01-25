// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StrategyCoreBasePlayerPawn.h"

AStrategyCoreBasePlayerPawn::AStrategyCoreBasePlayerPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
}
