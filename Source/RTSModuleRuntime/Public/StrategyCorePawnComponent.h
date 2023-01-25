// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularCharacter.h"
#include "Teams/LyraTeamAgentInterface.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "StrategyCorePawnComponent.generated.h"


class ALyraPlayerController;
class ALyraPlayerState;
class ULyraAbilitySystemComponent;
class UAbilitySystemComponent;
class ULyraPawnExtensionComponent;
class ULyraCameraComponent;

/**
 * Actor Component that handle StrategyCore logic , handle selection and task queue
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class STRATEGYCOREMODULERUNTIME_API ULyraStrategyCorePawnComponent : public ULyraPawnExtensionComponent
{
	GENERATED_BODY()

public:
	ULyraStrategyCorePawnComponent(const FObjectInitializer& ObjectInitializer);
	

	UFUNCTION(BlueprintPure, Category = "StrategyCore|Unit")
		bool GetIsSelected() const { return bIsSelected; }
private:
	bool bIsSelected = false;
};
