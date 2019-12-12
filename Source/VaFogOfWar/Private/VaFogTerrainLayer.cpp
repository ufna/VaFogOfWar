// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogTerrainLayer.h"

#include "VaFogBoundsVolume.h"
#include "VaFogController.h"
#include "VaFogDefines.h"

#include "Components/BillboardComponent.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"

AVaFogTerrainLayer::AVaFogTerrainLayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	if (SpriteComponent)
	{
		// Structure to hold one-time initialization
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> TextRenderTexture;
			FConstructorStatics()
				: TextRenderTexture(TEXT("/Engine/EditorResources/S_Terrain.S_Terrain"))
			{
			}
		};
		static FConstructorStatics ConstructorStatics;

		SpriteComponent->Sprite = ConstructorStatics.TextRenderTexture.Get();
	}
#endif

	LayerChannel = EVaFogLayerChannel::Terrain;
	bUseUpscaleBuffer = false;
	ZeroBufferValue = static_cast<uint8>(EVaFogHeightLevel::HL_1);

	InitialTerrainBuffer = nullptr;
	bUpdateRequired = false;
}

void AVaFogTerrainLayer::InitInternalBuffers()
{
	Super::InitInternalBuffers();

	InitialTerrainBuffer = new uint8[SourceBufferLength];
	FMemory::Memset(InitialTerrainBuffer, ZeroBufferValue, SourceBufferLength);

	LoadTerrainBufferFromTexture();

	// Apply initial buffer to source
	FMemory::Memcpy(SourceBuffer, InitialTerrainBuffer, SourceBufferLength);
}

void AVaFogTerrainLayer::CleanupInternalBuffers()
{
	if (InitialTerrainBuffer)
	{
		delete[] InitialTerrainBuffer;
		InitialTerrainBuffer = nullptr;
	}

	Super::CleanupInternalBuffers();
}

void AVaFogTerrainLayer::BeginPlay()
{
	// Link self buffer as source
	TerrainBuffer = SourceBuffer;

	// Apply all terrain blocking volumes into initial buffer
	Super::BeginPlay();
}

#if WITH_EDITOR
void AVaFogTerrainLayer::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	static const FName NAME_InitialTerrainTexture = FName(TEXT("InitialTerrainTexture"));

	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.Property->GetFName() == NAME_InitialTerrainTexture)
		{
			CleanupInternalBuffers();
			InitInternalBuffers();
		}
	}
}
#endif

void AVaFogTerrainLayer::UpdateLayer(bool bForceFullUpdate)
{
	if (bForceFullUpdate || bUpdateRequired)
	{
		UpdateBlockingVolumes();
		UpdateAgents();

		bUpdateRequired = false;
	}
}

EVaFogHeightLevel AVaFogTerrainLayer::GetHeightLevelAtLocation(const FVector& Location) const
{
	if (!BoundsVolume)
	{
		UE_LOG(LogVaFog, Warning, TEXT("[%s] Fog bounds volume is not registered yet, return default height level"), *VA_FUNC_LINE);
		return EVaFogHeightLevel::HL_1;
	}

	return GetHeightLevelAtAgentLocation(BoundsVolume->TransformWorldToLayer(Location));
}

EVaFogHeightLevel AVaFogTerrainLayer::GetHeightLevelAtAgentLocation(const FIntPoint& AgentLocation) const
{
	// @TODO Initial values should be valided before use https://github.com/ufna/VaFogOfWar/issues/68
	uint8 HeightLevelValue = SourceBuffer[AgentLocation.Y * SourceW + AgentLocation.X];
	return static_cast<EVaFogHeightLevel>(FMath::Clamp(FMath::RoundUpToPowerOfTwo(HeightLevelValue), static_cast<uint32>(EVaFogHeightLevel::HL_1), static_cast<uint32>(EVaFogHeightLevel::HL_8)));
}

void AVaFogTerrainLayer::LoadTerrainBufferFromTexture()
{
	UE_LOG(LogVaFog, Warning, TEXT("[%s] Update initial terrain buffer"), *VA_FUNC_LINE);

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
}

void AVaFogTerrainLayer::OnAddFogAgent(UVaFogAgentComponent* InFogAgent)
{
	bUpdateRequired = true;
}

void AVaFogTerrainLayer::OnRemoveFogAgent(UVaFogAgentComponent* InFogAgent)
{
	UpdateObstacle(InFogAgent, false);

	bUpdateRequired = true;
}

void AVaFogTerrainLayer::OnRemoveFogBlockingVolume(AVaFogBlockingVolume* InFogBlockingVolume)
{
	// Force update layer state for blocking volumes
	UpdateLayer(true);
}
