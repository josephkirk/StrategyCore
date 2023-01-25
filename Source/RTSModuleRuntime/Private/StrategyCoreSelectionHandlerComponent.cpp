// Fill out your copyright notice in the Description page of Project Settings.


#include "StrategyCoreSelectionHandlerComponent.h"

// Sets default values for this component's properties
UStrategyCoreSelectionHandlerComponent::UStrategyCoreSelectionHandlerComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this component to be initialized when the game staStrategyCore, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game staStrategyCore
void UStrategyCoreSelectionHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UStrategyCoreSelectionHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

