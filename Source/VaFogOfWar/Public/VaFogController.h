// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogTypes.h"

#include "Engine/Engine.h"

#include "VaFogController.generated.h"

class AVaFogBoundsVolume;
class UVaFogAgentComponent;
class AVaFogLayer;

UCLASS()
class VAFOGOFWAR_API UVaFogController : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Direct access to fog controller */
	static UVaFogController* Get(const UObject* WorldContextObject, EGetWorldErrorMode ErrorMode = EGetWorldErrorMode::Assert);

	void OnFogLayerAdded(AVaFogLayer* InFogLayer);
	void OnFogLayerRemoved(AVaFogLayer* InFogLayer);

	void OnFogAgentAdded(UVaFogAgentComponent* InFogAgent);
	void OnFogAgentRemoved(UVaFogAgentComponent* InFogAgent);

	/** Get fog layer by its channel */
	AVaFogLayer* GetFogLayer(EVaFogLayerChannel LayerChannel) const;

private:
	/** Registered fog layers */
	TArray<TWeakObjectPtr<AVaFogLayer>> FogLayers;
};
