// Copyright 2022, Jesus Bracho, All Rights Reserved.

#include "StrategyCoreCameraBoundsVolume.h"
#include "Components/PrimitiveComponent.h"

AStrategyCoreCameraBoundsVolume::AStrategyCoreCameraBoundsVolume()
{
    this->Tags.Add("OpenStrategyCoreCamera#CameraBounds");
    
    if (UPrimitiveComponent* PrimitiveComponent = this->FindComponentByClass<UPrimitiveComponent>())
    {
        PrimitiveComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName, false);
    }
}
