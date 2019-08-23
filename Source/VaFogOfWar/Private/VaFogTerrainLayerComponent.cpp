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
}

void UVaFogTerrainLayerComponent::OnRegister()
{
	Super::OnRegister();

	InitialTerrainBuffer = new uint8[SourceBufferLength];
	FMemory::Memset(InitialTerrainBuffer, ZeroBufferValue, SourceBufferLength);

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
						FMemory::Memcpy(InitialTerrainBuffer, TextureData, SourceBufferLength);
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
	uint8 HeightLevelValue = InitialTerrainBuffer[AgentLocation.Y * SourceW + AgentLocation.X];

	// @TODO Initial values should be valided before use https://github.com/ufna/VaFogOfWar/issues/68
	if (HeightLevelValue < static_cast<uint8>(EVaFogHeightLevel::HL_2))
	{
		return EVaFogHeightLevel::HL_1;
	}
	else if (HeightLevelValue < static_cast<uint8>(EVaFogHeightLevel::HL_3))
	{
		return EVaFogHeightLevel::HL_2;
	}
	else if (HeightLevelValue < static_cast<uint8>(EVaFogHeightLevel::HL_4))
	{
		return EVaFogHeightLevel::HL_3;
	}
	else if (HeightLevelValue < static_cast<uint8>(EVaFogHeightLevel::HL_5))
	{
		return EVaFogHeightLevel::HL_4;
	}
	else if (HeightLevelValue < static_cast<uint8>(EVaFogHeightLevel::HL_6))
	{
		return EVaFogHeightLevel::HL_5;
	}
	else if (HeightLevelValue < static_cast<uint8>(EVaFogHeightLevel::HL_7))
	{
		return EVaFogHeightLevel::HL_6;
	}
	else if (HeightLevelValue < static_cast<uint8>(EVaFogHeightLevel::HL_8))
	{
		return EVaFogHeightLevel::HL_7;
	}

	return EVaFogHeightLevel::HL_8;
}
