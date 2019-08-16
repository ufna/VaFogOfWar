// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogRadiusStrategy.h"

FVaFogRadiusStrategy::~FVaFogRadiusStrategy() = default;

int32 FVaFogRadiusStrategy_Circle::RadiusSquared(int32 CenterX, int32 CenterY, int32 X, int32 Y)
{
	return ((X - CenterX) * (X - CenterX) + (Y - CenterY) * (Y - CenterY));
}
