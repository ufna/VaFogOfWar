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

	OriginalBuffer = nullptr;
	OriginalBufferLength = 0;
	W = 0;
	H = 0;

	bDebugAgents = false;
	DebugAgentsColor = FColor::Red;
}

void UVaFogLayerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Cache texture size values
	CachedTextureResolution = FVaFogOfWarModule::Get().GetSettings()->FogLayerResolution;
	check(FMath::IsPowerOfTwo(CachedTextureResolution));
	OriginalRegion = FUpdateTextureRegion2D(0, 0, 0, 0, CachedTextureResolution, CachedTextureResolution);

	// Create texture buffer and initialize it
	check(!OriginalBuffer);
	W = (int32)OriginalRegion.Width;
	H = (int32)OriginalRegion.Height;
	OriginalBuffer = new uint8[W * H];
	OriginalBufferLength = W * H * sizeof(uint8);
	FMemory::Memzero(OriginalBuffer, OriginalBufferLength);

	OriginalTexture = UTexture2D::CreateTransient(OriginalRegion.Width, OriginalRegion.Height, EPixelFormat::PF_G8);
	OriginalTexture->CompressionSettings = TextureCompressionSettings::TC_Grayscale;
	OriginalTexture->SRGB = false;
	OriginalTexture->Filter = TextureFilter::TF_Trilinear;
	OriginalTexture->AddressX = TextureAddress::TA_Clamp;
	OriginalTexture->AddressY = TextureAddress::TA_Clamp;
	OriginalTexture->UpdateResource();

	UVaFogController::Get(this)->OnFogLayerAdded(this);
}

void UVaFogLayerComponent::UninitializeComponent()
{
	Super::UninitializeComponent();

	if (OriginalBuffer)
	{
		delete[] OriginalBuffer;
		OriginalBuffer = nullptr;
	}

	if (OriginalTexture)
	{
		OriginalTexture = nullptr;
	}

	if (UVaFogController::Get(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		UVaFogController::Get(this)->OnFogLayerRemoved(this);
	}
}

void UVaFogLayerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto FogVolume = UVaFogController::Get(this)->GetFogVolume();

	for (auto FogAgent : FogAgents)
	{
		FIntPoint AgentLocation = FogVolume->TransformWorldToLayer(FogAgent->GetOwner()->GetActorLocation());
		//UE_LOG(LogVaFog, Warning, TEXT("[%s] Agent [%s] location: %s"), *VA_FUNC_LINE, *FogAgent->GetName(), *AgentLocation.ToString());

		if (bDebugAgents)
		{
			DrawDebugSphere(GetWorld(), FogAgent->GetOwner()->GetActorLocation(), FogAgent->VisionRadius, 32, DebugAgentsColor, false, 0.0f);
		}

		check(AgentLocation.X >= 0 && AgentLocation.X < (int32)OriginalRegion.Width && AgentLocation.Y >= 0 && AgentLocation.Y < (int32)OriginalRegion.Height);
		OriginalBuffer[AgentLocation.Y * OriginalRegion.Width + AgentLocation.X] = 0xFF;
	}

	struct FTextureData
	{
		FTexture2DResource* Texture2DResource;
		FUpdateTextureRegion2D* Region;
		uint32 SrcPitch;
		uint8* SrcData;
	};

	// Copy original data fro GPU
	uint8* Buffer = new uint8[W * H];
	FMemory::Memcpy(Buffer, OriginalBuffer, OriginalBufferLength);

	FTextureData* TextureData = new FTextureData();
	TextureData->Texture2DResource = (FTexture2DResource*)OriginalTexture->Resource;
	TextureData->SrcPitch = OriginalRegion.Width;
	TextureData->SrcData = Buffer;
	TextureData->Region = &OriginalRegion;

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
