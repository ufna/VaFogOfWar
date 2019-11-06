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

	// Force terrain buffer to be not flipped as it's just a blocker level
	bNeedToSwitchVerticalAxis = false;

	InitialTerrainBuffer = new uint8[SourceBufferLength];
	FMemory::Memset(InitialTerrainBuffer, ZeroBufferValue, SourceBufferLength);

	// Check initial state and load it if necessary
	if (InitialTerrainTexture)
	{
		if (InitialTerrainTexture->PlatformData && InitialTerrainTexture->GetPixelFormat() != EPixelFormat::PF_Unknown)
		{
			int32 SizeX = InitialTerrainTexture->GetSizeX();
			int32 SizeY = InitialTerrainTexture->GetSizeY();
			EPixelFormat PixelFormat = InitialTerrainTexture->GetPixelFormat();
			int32 BytesPerPixel = (PixelFormat == EPixelFormat::PF_G8) ? sizeof(uint8) : static_cast<int32>(PixelFormat);

			// Check that initial texture has right size
			if (SizeX == SourceW && SizeY == SourceH)
			{
				if (BytesPerPixel == sizeof(uint8))
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
					UE_LOG(LogVaFog, Warning, TEXT("[%s] Layer [%s] InitialTerrainTexture has wrong BytesPerPixel: %d, expected: %d"), *VA_FUNC_LINE, *GetName(), BytesPerPixel, sizeof(uint8));
				}
			}
			else
			{
				UE_LOG(LogVaFog, Warning, TEXT("[%s] Layer [%s] InitialTerrainTexture has wrong size: %d x %d, expected: %d x %d"), *VA_FUNC_LINE, *GetName(), SizeX, SizeY, SourceW, SourceH);
			}
		}
		else
		{
#if WITH_EDITORONLY_DATA
			UE_LOG(LogVaFog, Warning, TEXT("[%s] Layer [%s] Can't load PlatformData, try to process Source texture"), *VA_FUNC_LINE, *GetName());

			int32 SizeX = InitialTerrainTexture->Source.GetSizeX();
			int32 SizeY = InitialTerrainTexture->Source.GetSizeY();
			int32 BytesPerPixel = InitialTerrainTexture->Source.GetBytesPerPixel();

			if (SizeX == SourceW && SizeY == SourceH)
			{
				uint8* TextureData = static_cast<uint8*>(InitialTerrainTexture->Source.LockMip(0));
				if (TextureData)
				{
					// Load each forth pixel (r channel)
					for (int x = 0; x < SourceW; ++x)
					{
						for (int y = 0; y < SourceH; ++y)
						{
							InitialTerrainBuffer[y * SourceW + x] = TextureData[y * SourceW * BytesPerPixel + x * BytesPerPixel];
						}
					}

					FMemory::Memcpy(SourceBuffer, InitialTerrainBuffer, SourceBufferLength);

					UE_LOG(LogVaFog, Warning, TEXT("[%s] Layer [%s] Initial terrain buffer successfully loaded from Source texture"), *VA_FUNC_LINE, *GetName());
				}

				InitialTerrainTexture->Source.UnlockMip(0);
			}
			else
			{
				UE_LOG(LogVaFog, Warning, TEXT("[%s] Layer [%s] InitialTerrainTexture Source has wrong size: %d x %d, expected: %d x %d"), *VA_FUNC_LINE, *GetName(), SizeX, SizeY, SourceW, SourceH);
			}
#else
			UE_LOG(LogVaFog, Error, TEXT("[%s] Layer [%s] Can't load platform data for initial terrain layer"), *VA_FUNC_LINE, *GetName());
#endif
		}
	}

	// Link self buffer as source
	TerrainBuffer = SourceBuffer;
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
