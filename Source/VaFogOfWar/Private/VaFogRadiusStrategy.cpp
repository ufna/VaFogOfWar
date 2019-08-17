// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogRadiusStrategy.h"

FVaFogRadiusStrategy::~FVaFogRadiusStrategy() = default;

bool FVaFogRadiusStrategy_Circle::IsInRadius(int32 CenterX, int32 CenterY, int32 Radius, int32 X, int32 Y)
{
	return ((X - CenterX) * (X - CenterX) + (Y - CenterY) * (Y - CenterY)) < FMath::Square(Radius);
}
