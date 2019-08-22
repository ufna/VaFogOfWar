// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogTerrainLayerComponent.h"

#include "VaFogDefines.h"

#include "Engine/Texture2D.h"

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
		// Check that initial texture has right size
		if (InitialTerrainTexture->Source.GetSizeX() == SourceW && InitialTerrainTexture->Source.GetSizeY() == SourceH)
		{
			if (InitialTerrainTexture->GetPixelFormat() == EPixelFormat::PF_G8)
			{
				// Check that mip settings are set as TMGS_NoMipmaps
				if (InitialTerrainTexture->PlatformData->Mips.Num() == 1)
				{
					uint8* TextureData = static_cast<uint8*>(InitialTerrainTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_ONLY));
					if (TextureData)
					{
						FMemory::Memcpy(SourceBuffer, TextureData, SourceBufferLength);
					}
					else
					{
						UE_LOG(LogVaFog, Error, TEXT("[%s] Layer [%s] Can't lock InitialTerrainTexture to read its pixels: %s"), *VA_FUNC_LINE, *GetName(), *InitialTerrainTexture->GetName());
					}

					InitialTerrainTexture->PlatformData->Mips[0].BulkData.Unlock();
				}
				else
				{
					UE_LOG(LogVaFog, Error, TEXT("[%s] Layer [%s] InitialTerrainTexture should be set as TMGS_NoMipmaps"), *VA_FUNC_LINE, *GetName());
				}
			}
			else
			{
				UE_LOG(LogVaFog, Error, TEXT("[%s] Layer [%s] InitialTerrainTexture has wrong pixel format: %d, expected: %d"), *VA_FUNC_LINE, *GetName(),
					static_cast<int32>(InitialTerrainTexture->GetPixelFormat()), static_cast<int32>(EPixelFormat::PF_G8));
			}
		}
		else
		{
			UE_LOG(LogVaFog, Error, TEXT("[%s] Layer [%s] InitialTerrainTexture has wrong size: %d x %d, expected: %d x %d"), *VA_FUNC_LINE, *GetName(),
				InitialTerrainTexture->Source.GetSizeX(), InitialTerrainTexture->Source.GetSizeY(),
				SourceW, SourceH);
		}
	}

	Super::BeginPlay();
}
