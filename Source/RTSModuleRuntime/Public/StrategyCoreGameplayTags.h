// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_CameraDrag)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_CameraMove)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_CameraEdgeScroll)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_CameraRotateAxis)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_CameraTurnLeft)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_CameraTurnRight)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_CameraZoom)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Camera_EdgeScroll)
struct STRATEGYCOREMODULERUNTIME_API FStrategyCoreGameplayTags : public FGameplayTagNativeAdder
{
public:
	//Tag used for Fire Damage
	FGameplayTag Damage_Fire;

public:
//
	//Static accessor for accessing the tags. Access tags using:
	//FStrategyCoreGameplayTags::Get().Damage_Fire for example.
	FORCEINLINE static const FStrategyCoreGameplayTags& Get() { return StrategyCoreGameplayTags; }

protected:
//	//Called to register and assign the native tags
	virtual void AddTags() override
	{
		UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

		Damage_Fire = Manager.AddNativeGameplayTag(TEXT("Status.Damage.Fire"));
		
	}
//
//
private:
	//Private static object for the global tags. Use the Get() function to access externally.
	static FStrategyCoreGameplayTags StrategyCoreGameplayTags;
};