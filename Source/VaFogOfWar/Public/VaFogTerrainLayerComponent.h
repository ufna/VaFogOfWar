// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogLayerComponent.h"

#include "VaFogTerrainLayerComponent.generated.h"

/** 
 * Pre-configured layer component for handling terrain and obstacles
 */
UCLASS(ClassGroup = (VAFogOfWar), editinlinenew, meta = (BlueprintSpawnableComponent))
class VAFOGOFWAR_API UVaFogTerrainLayerComponent : public UVaFogLayerComponent
{
	GENERATED_UCLASS_BODY()

public:
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void BeginPlay() override;

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
