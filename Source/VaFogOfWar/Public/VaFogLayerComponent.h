// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogTypes.h"

#include "Components/ActorComponent.h"

#include "VaFogLayerComponent.generated.h"

UCLASS(ClassGroup = (VAFogOfWar), editinlinenew, meta = (BlueprintSpawnableComponent))
class VAFOGOFWAR_API UVaFogLayerComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:
	/**  */
	UPROPERTY(EditDefaultsOnly)
	EVaFogLayerChannel LayerChannel;
};
