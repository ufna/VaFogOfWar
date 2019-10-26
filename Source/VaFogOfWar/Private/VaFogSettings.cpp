// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogSettings.h"

UVaFogSettings::UVaFogSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FogLayerResolution = 128;
}

void UVaFogSettings::PostInitProperties()
{
	Super::PostInitProperties();

	SanatizeFogLayerResolution();
}

#if WITH_EDITOR
void UVaFogSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	SanatizeFogLayerResolution();

	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UVaFogSettings, FogLayerResolution) &&
			PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
		{
			// @TODO Update in-editor fog layers
		}
	}
}
#endif

void UVaFogSettings::SanatizeFogLayerResolution()
{
	static const int32 MaxFogLayerResolution = 512;
	static const int32 MinFogLayerResolution = 64;
	FogLayerResolution = FMath::Clamp(int32(FMath::RoundUpToPowerOfTwo(FogLayerResolution)), MinFogLayerResolution, MaxFogLayerResolution);
}
