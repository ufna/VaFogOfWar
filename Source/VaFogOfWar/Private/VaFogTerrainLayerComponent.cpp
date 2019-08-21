// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogTerrainLayerComponent.h"

UVaFogTerrainLayerComponent::UVaFogTerrainLayerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LayerChannel = EVaFogLayerChannel::Terrain;
	bUseUpscaleBuffer = false;
	ZeroBufferValue = static_cast<uint8>(EVaFogHeightLevel::HL_1);
}

void UVaFogTerrainLayerComponent::BeginPlay()
{
	// Check initial state and load it if necessary
	if (InitialTerrainTexture)
	{
		// @TODO
	}

	Super::BeginPlay();
}
