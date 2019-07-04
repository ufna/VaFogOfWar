// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogLayerComponent.h"

#include "VaFogAgentComponent.h"
#include "VaFogBoundsVolume.h"
#include "VaFogController.h"
#include "VaFogDefines.h"
#include "VaFogOfWar.h"
#include "VaFogSettings.h"

#include "DrawDebugHelpers.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/Material.h"

UVaFogLayerComponent::UVaFogLayerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_DuringPhysics;

	SourceBuffer = nullptr;
	SourceBufferLength = 0;
	SourceW = 0;
	SourceH = 0;

	bDebugAgents = false;
	DebugAgentsColor = FColor::Red;
	bDebugSourceTexture = false;
}

void UVaFogLayerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Cache texture size values
	int32 CachedTextureResolution = FVaFogOfWarModule::Get().GetSettings()->FogLayerResolution;
	check(FMath::IsPowerOfTwo(CachedTextureResolution));
	SourceUpdateRegion = FUpdateTextureRegion2D(0, 0, 0, 0, CachedTextureResolution, CachedTextureResolution);

	// Create texture buffer and initialize it
	check(!SourceBuffer);
	SourceW = (int32)SourceUpdateRegion.Width;
	SourceH = (int32)SourceUpdateRegion.Height;
	SourceBuffer = new uint8[SourceW * SourceH];
	SourceBufferLength = SourceW * SourceH * sizeof(uint8);
	FMemory::Memzero(SourceBuffer, SourceBufferLength);

	// Prepare debug textures if required
	if (bDebugSourceTexture)
	{
		SourceTexture = UTexture2D::CreateTransient(SourceUpdateRegion.Width, SourceUpdateRegion.Height, EPixelFormat::PF_G8);
		SourceTexture->CompressionSettings = TextureCompressionSettings::TC_Grayscale;
		SourceTexture->SRGB = false;
		SourceTexture->Filter = TextureFilter::TF_Nearest;
		SourceTexture->AddressX = TextureAddress::TA_Clamp;
		SourceTexture->AddressY = TextureAddress::TA_Clamp;
		SourceTexture->UpdateResource();
	}

	UVaFogController::Get(this)->OnFogLayerAdded(this);
}

void UVaFogLayerComponent::UninitializeComponent()
{
	Super::UninitializeComponent();

	if (SourceBuffer)
	{
		delete[] SourceBuffer;
		SourceBuffer = nullptr;
	}

	if (SourceTexture)
	{
		SourceTexture = nullptr;
	}

	if (UVaFogController::Get(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		UVaFogController::Get(this)->OnFogLayerRemoved(this);
	}
}

void UVaFogLayerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateAgents();

	if (bDebugSourceTexture)
	{
		UpdateSourceTexture();
	}
}

void UVaFogLayerComponent::UpdateAgents()
{
	auto FogVolume = UVaFogController::Get(this)->GetFogVolume();

	for (auto FogAgent : FogAgents)
	{
		FIntPoint AgentLocation = FogVolume->TransformWorldToLayer(FogAgent->GetOwner()->GetActorLocation());
		//UE_LOG(LogVaFog, Warning, TEXT("[%s] Agent [%s] location: %s"), *VA_FUNC_LINE, *FogAgent->GetName(), *AgentLocation.ToString());

		if (bDebugAgents)
		{
			DrawDebugSphere(GetWorld(), FogAgent->GetOwner()->GetActorLocation(), FogAgent->VisionRadius, 32, DebugAgentsColor, false, 0.0f);
		}

		check(AgentLocation.X >= 0 && AgentLocation.X < (int32)SourceUpdateRegion.Width && AgentLocation.Y >= 0 && AgentLocation.Y < (int32)SourceUpdateRegion.Height);
		SourceBuffer[AgentLocation.Y * SourceUpdateRegion.Width + AgentLocation.X] = 0xFF;
	}
}

void UVaFogLayerComponent::AddFogAgent(UVaFogAgentComponent* FogAgent)
{
	FogAgents.AddUnique(FogAgent);
}

void UVaFogLayerComponent::RemoveFogAgent(UVaFogAgentComponent* InFogAgent)
{
	int32 NumRemoved = FogAgents.Remove(InFogAgent);
	if (NumRemoved == 0)
	{
		UE_LOG(LogVaFog, Error, TEXT("[%s] No cached data found for: %s"), *VA_FUNC_LINE, *InFogAgent->GetName());
	}
}

void UVaFogLayerComponent::UpdateSourceTexture()
{
	if (SourceTexture)
	{
		struct FTextureData
		{
			FTexture2DResource* Texture2DResource;
			FUpdateTextureRegion2D* Region;
			uint32 SrcPitch;
			uint8* SrcData;
		};

		// Copy original data fro GPU
		uint8* Buffer = new uint8[SourceW * SourceH];
		FMemory::Memcpy(Buffer, SourceBuffer, SourceBufferLength);

		FTextureData* TextureData = new FTextureData();
		TextureData->Texture2DResource = (FTexture2DResource*)SourceTexture->Resource;
		TextureData->SrcPitch = SourceUpdateRegion.Width;
		TextureData->SrcData = Buffer;
		TextureData->Region = &SourceUpdateRegion;

		ENQUEUE_RENDER_COMMAND(UpdateTexture)
		(
			[TextureData](FRHICommandListImmediate& RHICmdList) {
				int32 CurrentFirstMip = TextureData->Texture2DResource->GetCurrentFirstMip();
				if (CurrentFirstMip <= 0)
				{
					RHIUpdateTexture2D(
						TextureData->Texture2DResource->GetTexture2DRHI(),
						0 - CurrentFirstMip,
						*TextureData->Region,
						TextureData->SrcPitch,
						TextureData->SrcData);
				}
				delete[] TextureData->SrcData;
				delete TextureData;
			});
	}
	else
	{
		UE_LOG(LogVaFog, Error, TEXT("[%s] SourceTexture is invalid"), *VA_FUNC_LINE);
	}
}