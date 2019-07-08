// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaFogTypes.h"

#include "Components/ActorComponent.h"

#include "VaFogLayerComponent.generated.h"

class UTextureRenderTarget2D;
class UTexture2D;
class UMaterialInterface;
struct FUpdateTextureRegion2D;

class UVaFogAgentComponent;

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

UCLASS(ClassGroup = (VAFogOfWar), editinlinenew, meta = (BlueprintSpawnableComponent))
class VAFOGOFWAR_API UVaFogLayerComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	/** Process agents info and update FoW map */
	void UpdateAgents();

	/** Process manual upscaling from 128 to 512 */
	void UpdateUpscaleBuffer();

private:
	/** Rasterize circle with Bresenham's Midpoint circle algorithm, see https://en.wikipedia.org/wiki/Midpoint_circle_algorithm */
	void DrawCircle(int32 CenterX, int32 CenterY, int32 Radius);
	void Plot4Points(int32 CenterX, int32 CenterY, int32 X, int32 Y);
	void DrawHorizontalLine(int32 x0, int32 y0, int32 x1);

	/** Read pixel with desired position and constuct texel based on its neighbors */
	FFogTexel2x2 FetchTexelFromSource(int32 W, int32 H);

public:
	/** Defines which refresh logic will be used: permanent drawing or runtime visible area */
	UPROPERTY(EditAnywhere, Category = "Fog of War")
	EVaFogLayerChannel LayerChannel;

public:
	void AddFogAgent(UVaFogAgentComponent* InFogAgent);
	void RemoveFogAgent(UVaFogAgentComponent* InFogAgent);

protected:
	/** Registered fog agents for layer */
	UPROPERTY()
	TArray<UVaFogAgentComponent*> FogAgents;

private:
	/** Original layer texture on CPU */
	uint8* SourceBuffer;

	/** Upscaled layer texture on CPU */
	uint8* UpscaleBuffer;

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
	// Debug

protected:
	/** Show agents vision radius for this layer */
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDebugAgents;

	/** Color to draw */
	UPROPERTY(EditAnywhere, Category = "Debug")
	FColor DebugAgentsColor;

	/** Enable source and upscale buffer to texture drawing */
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDebugBuffers;

public:
	/** Low-res FoW source buffer as image (check bDebugSourceTexture) */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Fog of War")
	UTexture2D* SourceTexture;

	/** Upscaled FoW buffer as image (check bDebugUpscaleTexture) */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Fog of War")
	UTexture2D* UpscaleTexture;

protected:
	/** Render buffer into debug texture */
	void UpdateTextureFromBuffer(UTexture2D* DestinationTexture, uint8* SrcBuffer, int32 SrcBufferLength, FUpdateTextureRegion2D& UpdateTextureRegion);
};
