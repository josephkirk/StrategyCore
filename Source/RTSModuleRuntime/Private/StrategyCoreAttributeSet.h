// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/Attributes/LyraAttributeSet.h"
#include "NativeGameplayTags.h"
#include "StrategyCoreAttributeSet.generated.h"


class AActor;
class UAbilitySystemComponent;
class ULyraAbilitySystemComponent;
class UObject;
class UWorld;
struct FGameplayEffectSpec;

/**
 * 
 */
UCLASS(BlueprintType)
class STRATEGYCOREMODULERUNTIME_API UStrategyCoreAttributeSet : public ULyraAttributeSet
{
	GENERATED_BODY()
public:
	UStrategyCoreAttributeSet();

	const float MAXPOPULATION = 1000.0f;
	const float MAXMINERAL = 9999.0f;
	const float MAXGAS = 9999.0f;


	ATTRIBUTE_ACCESSORS(ThisClass, Population);
	ATTRIBUTE_ACCESSORS(ThisClass, PopulationCap);
	ATTRIBUTE_ACCESSORS(ThisClass, Mineral);
	ATTRIBUTE_ACCESSORS(ThisClass, Gas);

	//~UAttributeSet interface
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//~End of UAttributeSet interface
protected:

	UFUNCTION()
		void OnRep_Population(const FGameplayAttributeData& OldValue);

	UFUNCTION()
		void OnRep_PopulationCap(const FGameplayAttributeData& OldValue);

	UFUNCTION()
		void OnRep_Mineral(const FGameplayAttributeData& OldValue);

	UFUNCTION()
		void OnRep_Gas(const FGameplayAttributeData& OldValue);

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
private:

	// Unit population cap
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PopulationCap, Category = "StrategyCoreGame", Meta = (AllowPrivateAccess = true))
		FGameplayAttributeData PopulationCap;

	// current unit population count
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Population, Category = "StrategyCoreGame", Meta = (AllowPrivateAccess = true))
		FGameplayAttributeData Population;

	// Current Mineral Amount
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mineral, Category = "StrategyCoreGame", Meta = (AllowPrivateAccess = true))
		FGameplayAttributeData Mineral;

	// Current Gas Amount
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Gas, Category = "StrategyCoreGame", Meta = (AllowPrivateAccess = true))
		FGameplayAttributeData Gas;

};
