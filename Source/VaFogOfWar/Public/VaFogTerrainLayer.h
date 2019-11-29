// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogLayer.h"

#include "VaFogTerrainLayer.generated.h"

/** 
 * Pre-configured layer for handling terrain and obstacles
 */
UCLASS()
class VAFOGOFWAR_API AVaFogTerrainLayer : public AVaFogLayer
{
	GENERATED_BODY()

public:
	AVaFogTerrainLayer(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;

	/** Get initial terrain state at location */
	UFUNCTION(BlueprintCallable, Category = "VaFog|Terrain")
	EVaFogHeightLevel GetHeightLevelAtLocation(const FVector& Location) const;

	/** Get initial terrain state at agent (internal) location */
	UFUNCTION(BlueprintCallable, Category = "VaFog|Terrain")
	EVaFogHeightLevel GetHeightLevelAtAgentLocation(const FIntPoint& AgentLocation) const;

public:
	/** Initial terrain state to be applied into the source buffer */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fog of War Terrain")
	UTexture2D* InitialTerrainTexture;

protected:
	/** Initial terrain buffer for navigation */
	uint8* InitialTerrainBuffer;
};
