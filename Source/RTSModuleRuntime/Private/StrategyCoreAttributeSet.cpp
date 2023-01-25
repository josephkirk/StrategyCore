// Fill out your copyright notice in the Description page of Project Settings.


#include "StrategyCoreAttributeSet.h"
#include "Net/UnrealNetwork.h"

UStrategyCoreAttributeSet::UStrategyCoreAttributeSet()
	: PopulationCap(15.0f)
	, Population(10.0f)
	, Mineral(100.0f)
	, Gas(0.0f)
{
}

void UStrategyCoreAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Population, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Mineral, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Gas, COND_None, REPNOTIFY_Always);
}

void UStrategyCoreAttributeSet::OnRep_Population(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Population, OldValue);
}


void UStrategyCoreAttributeSet::OnRep_PopulationCap(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Population, OldValue);
}


void UStrategyCoreAttributeSet::OnRep_Mineral(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Mineral, OldValue);
}

void UStrategyCoreAttributeSet::OnRep_Gas(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Gas, OldValue);
}

void UStrategyCoreAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UStrategyCoreAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UStrategyCoreAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetPopulationAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetPopulationCap());
	}
	else if (Attribute == GetPopulationCapAttribute())
	{
		NewValue = FMath::Clamp(NewValue, GetPopulation(), MAXPOPULATION);
	}
	else if (Attribute == GetMineralAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MAXMINERAL);
	}
	else if (Attribute == GetGasAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MAXGAS);
	}
}
