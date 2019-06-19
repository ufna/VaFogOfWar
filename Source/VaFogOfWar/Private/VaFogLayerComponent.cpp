// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogLayerComponent.h"

#include "VaFogDefines.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Materials/Material.h"
#include "UObject/ConstructorHelpers.h"

UVaFogLayerComponent::UVaFogLayerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("MaterialInstanceConstant'/VaFogOfWar/Materials/MI_Blur.MI_Blur'"));
	BlurMaterial = Material.Object;
}
