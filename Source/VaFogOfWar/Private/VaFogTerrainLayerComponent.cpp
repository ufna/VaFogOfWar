// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogTerrainLayerComponent.h"

#include "VaFogBoundsVolume.h"
#include "VaFogController.h"
#include "VaFogDefines.h"

#include "Engine/Texture2D.h"

UVaFogTerrainLayerComponent::UVaFogTerrainLayerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LayerChannel = EVaFogLayerChannel::Terrain;
	bUseUpscaleBuffer = false;
	ZeroBufferValue = static_cast<uint8>(EVaFogHeightLevel::HL_1);

	InitialTerrainBuffer = nullptr;

	InitialTerrainArray.AddZeroed(128*128);
}

void UVaFogTerrainLayerComponent::OnRegister()
{
	Super::OnRegister();

	InitialTerrainBuffer = new uint8[SourceBufferLength];
	FMemory::Memset(InitialTerrainBuffer, ZeroBufferValue, SourceBufferLength);

	// Check initial state and load it if necessary
	if (InitialTerrainTexture)
	{
		int32 SizeX = InitialTerrainTexture->GetSizeX();
		int32 SizeY = InitialTerrainTexture->GetSizeY();
		EPixelFormat PixelFormat = InitialTerrainTexture->GetPixelFormat();
		uint8 BytesPerPixel = (PixelFormat == EPixelFormat::PF_G8) ? sizeof(uint8) : static_cast<int32>(PixelFormat);

		// Check that initial texture has right size
		if (SizeX == SourceW && SizeY == SourceH)
		{
			if (BytesPerPixel == sizeof(uint8))
			{
				uint8* TextureData = static_cast<uint8*>(InitialTerrainTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_ONLY));

				if (TextureData)
				{
					FMemory::Memcpy(InitialTerrainArray.GetData(), TextureData, SourceBufferLength);
					/*FMemory::Memcpy(SourceBuffer, TextureData, SourceBufferLength);
					FMemory::Memcpy(InitialTerrainBuffer, TextureData, SourceBufferLength);*/
				}
				else
				{
					UE_LOG(LogVaFog, Error, TEXT("[%s] Layer [%s] Can't lock InitialTerrainTexture to read its pixels: %s"), *VA_FUNC_LINE, *GetName(), *InitialTerrainTexture->GetName());
				}

				InitialTerrainTexture->PlatformData->Mips[0].BulkData.Unlock();
			}
			else
			{
				UE_LOG(LogVaFog, Warning, TEXT("[%s] Layer [%s] InitialTerrainTexture has wrong BytesPerPixel: %d, expected: %d"), *VA_FUNC_LINE, *GetName(), BytesPerPixel, sizeof(uint8));
			}
		}
		else
		{
			UE_LOG(LogVaFog, Warning, TEXT("[%s] Layer [%s] InitialTerrainTexture has wrong size: %d x %d, expected: %d x %d"), *VA_FUNC_LINE, *GetName(), SizeX, SizeY, SourceW, SourceH);
		}
	}

	FMemory::Memcpy(InitialTerrainBuffer, InitialTerrainArray.GetData(), SourceBufferLength);
	FMemory::Memcpy(SourceBuffer, InitialTerrainArray.GetData(), SourceBufferLength);
}

void UVaFogTerrainLayerComponent::OnUnregister()
{
	if (InitialTerrainBuffer)
	{
		delete[] InitialTerrainBuffer;
		InitialTerrainBuffer = nullptr;
	}

	Super::OnUnregister();
}

EVaFogHeightLevel UVaFogTerrainLayerComponent::GetHeightLevelAtLocation(const FVector& Location) const
{
	auto FogVolume = UVaFogController::Get(this)->GetFogVolume();
	if (FogVolume)
	{
		return GetHeightLevelAtAgentLocation(FogVolume->TransformWorldToLayer(Location));
	}

	UE_LOG(LogVaFog, Warning, TEXT("[%s] Fog bounds volume is not registered yet, return default height level"), *VA_FUNC_LINE);
	return EVaFogHeightLevel::HL_1;
}

EVaFogHeightLevel UVaFogTerrainLayerComponent::GetHeightLevelAtAgentLocation(const FIntPoint& AgentLocation) const
{
	// @TODO Initial values should be valided before use https://github.com/ufna/VaFogOfWar/issues/68
	uint8 HeightLevelValue = InitialTerrainBuffer[AgentLocation.Y * SourceW + AgentLocation.X];
	return static_cast<EVaFogHeightLevel>(FMath::Clamp(FMath::RoundUpToPowerOfTwo(HeightLevelValue), static_cast<uint32>(EVaFogHeightLevel::HL_1), static_cast<uint32>(EVaFogHeightLevel::HL_8)));
}
