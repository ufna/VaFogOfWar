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

public:
	/** Defines which refresh logic will be used: permanent drawing or runtime visible area */
	UPROPERTY(EditDefaultsOnly, Category = "Fog of War")
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
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bDebugAgents;

	/** Color to draw */
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	FColor DebugAgentsColor;

	/** Enable source and upscale buffer to texture drawing */
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
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
