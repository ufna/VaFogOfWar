// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogTypes.h"

#include "VaFogRadiusStrategy.h"

#include "GameFramework/Actor.h"
#include "RHI.h"

#include "VaFogLayer.generated.h"

class UTextureRenderTarget2D;
class UTexture2D;
class UMaterialInterface;

class AVaFogBoundsVolume;
class UVaFogAgentComponent;
class AVaFogTerrainLayer;
class AVaFogBlockingVolume;

struct FFogTexel2x2
{
	uint8 p11, p12;
	uint8 p21, p22;

	bool operator==(const FFogTexel2x2& p) const
	{
		return p11 == p.p11 &&
			   p12 == p.p12 &&
			   p21 == p.p21 &&
			   p22 == p.p22;
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("%d %d %d %d"), p11, p12, p21, p22);
	}
};

struct FFogTexel4x4
{
	uint8 pixels[4][4];
};

struct FFogOctantTransform
{
	int8 xx, xy, yx, yy;

	bool operator==(const FFogOctantTransform& p) const
	{
		return xx == p.xx &&
			   xy == p.xy &&
			   yx == p.yx &&
			   yy == p.yy;
	}
};

struct FFogDrawContext
{
	uint8* TargetBuffer;
	int32 CenterX;
	int32 CenterY;
	int32 Radius;
	EVaFogRadiusStrategy RadiusStrategy;
	EVaFogHeightLevel HeightLevel;
	uint8 RevealLevel;

	FFogDrawContext()
		: TargetBuffer(nullptr)
		, CenterX(0)
		, CenterY(0)
		, Radius(1)
		, RadiusStrategy(EVaFogRadiusStrategy::Circle)
		, HeightLevel(EVaFogHeightLevel::HL_1)
		, RevealLevel(0xFF)
	{
	}
};

UCLASS(Blueprintable, BlueprintType)
class VAFOGOFWAR_API AVaFogLayer : public AActor
{
	GENERATED_BODY()

public:
	AVaFogLayer(const FObjectInitializer& ObjectInitializer);

	virtual void PostLoad() override;
	virtual void PostActorCreated() override;
	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;
	virtual void BeginPlay() override;

protected:
	virtual void InitInternalBuffers();
	virtual void CleanupInternalBuffers();

public:
	virtual void Tick(float DeltaTime) override;

	/** Update whole layer state */
	virtual void UpdateLayer(bool bForceFullUpdate = false);

protected:
	/** Flush buffers to textures */
	void UpdateBuffers();

	/** Process agents info and update FoW map */
	void UpdateAgents();

	/** Process volumes info and update FoW map */
	void UpdateBlockingVolumes();

	/** Update single obstacle agent as event-based process */
	void UpdateObstacle(UVaFogAgentComponent* FogAgent, bool bObstacleIsActive);

	/** Process manual upscaling from 128 to 512 */
	void UpdateUpscaleBuffer();

private:
	/** Draw circle shaded with obstacles: http://www.adammil.net/blog/v125_Roguelike_Vision_Algorithms.html */
	void DrawVisionCircle(const FFogDrawContext& DrawContext);

	/** Based on Shadowcasting algorithm from SquidLib by Eben Howard, http://www.roguebasin.com/index.php?title=Improved_Shadowcasting_in_Java */
	void DrawFieldOfView(const FFogDrawContext& DrawContext, int32 Y, float Start, float End, FFogOctantTransform Transform);

	/** Set desired point as visible in TargetBuffer */
	void Reveal(const FFogDrawContext& DrawContext, int32 X, int32 Y);

	/** Check obstacle buffer in desired point */
	bool IsBlocked(int32 X, int32 Y, EVaFogHeightLevel HeightLevel);

	/** Rasterize circle with Bresenham's Midpoint circle algorithm, see https://en.wikipedia.org/wiki/Midpoint_circle_algorithm */
	void DrawCircle(const FFogDrawContext& DrawContext);
	void Plot4Points(const FFogDrawContext& DrawContext, int32 X, int32 Y);
	void DrawHorizontalLine(uint8* TargetBuffer, int32 x0, int32 y0, int32 x1);

	/** Read pixel with desired position and constuct texel based on its neighbors */
	FFogTexel2x2 FetchTexelFromSource(int32 W, int32 H);

public:
	/** Defines which refresh logic will be used: permanent drawing or runtime visible area */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Fog of War")
	EVaFogLayerChannel LayerChannel;

	/** Layer that provides world information */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Fog of War")
	AVaFogTerrainLayer* TerrainLayer;

	/** Fog bounds volume we're linked in */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Fog of War")
	AVaFogBoundsVolume* BoundsVolume;

public:
	void AddFogAgent(UVaFogAgentComponent* InFogAgent);
	void RemoveFogAgent(UVaFogAgentComponent* InFogAgent);

	void AddFogBlockingVolume(AVaFogBlockingVolume* InFogBlockingVolume);
	void RemoveFogBlockingVolume(AVaFogBlockingVolume* InFogBlockingVolume);

protected:
	virtual void OnAddFogAgent(UVaFogAgentComponent* InFogAgent){};
	virtual void OnRemoveFogAgent(UVaFogAgentComponent* InFogAgent){};
	virtual void OnAddFogBlockingVolume(AVaFogBlockingVolume* InFogBlockingVolume){};
	virtual void OnRemoveFogBlockingVolume(AVaFogBlockingVolume* InFogBlockingVolume){};

protected:
	/** Registered fog agents for layer */
	UPROPERTY(VisibleAnywhere, Category = "Fog of War")
	TArray<UVaFogAgentComponent*> FogAgents;

	/** Registered fog blocking volumes for layer */
	TArray<TWeakObjectPtr<AVaFogBlockingVolume>> FogBlockingVolumes;

	/** Radius strategy instances */
	TMap<EVaFogRadiusStrategy, FVaFogRadiusStrategyRef> RadiusStrategies;

	/** Is upscaling enabled or original buffer used */
	bool bUseUpscaleBuffer;

	/** Default source buffer state */
	uint8 ZeroBufferValue;

	/** Original layer texture on CPU */
	uint8* SourceBuffer;

	/** Upscaled layer texture on CPU */
	uint8* UpscaleBuffer;

	/** Separate buffer used to track obstacles and height levels (same size as source buffer) */
	uint8* TerrainBuffer;

	/** Cached data helpers for original texture */
	int32 SourceW;
	int32 SourceH;
	int32 SourceBufferLength;
	FUpdateTextureRegion2D SourceUpdateRegion;

	/** Cached data helpers for upscale texture */
	int32 UpscaleW;
	int32 UpscaleH;
	int32 UpscaleBufferLength;
	FUpdateTextureRegion2D UpscaleUpdateRegion;

	//////////////////////////////////////////////////////////////////////////
	// Gameplay

public:
	/** Is location is revealed on layer  */
	UFUNCTION(BlueprintCallable, Category = "VaFog|FogLayer")
	bool IsLocationRevealed(const FVector& InLocation) const;

	//////////////////////////////////////////////////////////////////////////
	// Debug

protected:
	/** Show agents vision radius for this layer */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bDebugAgents;

	/** Color to draw */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	FColor DebugAgentsColor;

	/** Enable source and upscale buffer to texture drawing */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bDebugBuffers;

public:
	/** Low-res FoW source buffer as image (check bDebugBuffers) */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Fog of War")
	UTexture2D* SourceTexture;

	/** Upscaled FoW buffer as image (check bDebugUpscaleTexture) */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Fog of War")
	UTexture2D* UpscaleTexture;

protected:
	/** Render buffer into debug texture */
	void UpdateTextureFromBuffer(UTexture2D* DestinationTexture, uint8* SrcBuffer, int32 SrcBufferLength, FUpdateTextureRegion2D& UpdateTextureRegion);

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	UBillboardComponent* SpriteComponent;
#endif
};
