// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogRadiusStrategy.h"

FVaFogRadiusStrategy::~FVaFogRadiusStrategy() = default;

bool FVaFogRadiusStrategy_Circle::IsInRadius(int32 CenterX, int32 CenterY, int32 Radius, int32 X, int32 Y)
{
	return ((X - CenterX) * (X - CenterX) + (Y - CenterY) * (Y - CenterY)) < FMath::Square(Radius);
}

bool FVaFogRadiusStrategy_Square::IsInRadius(int32 CenterX, int32 CenterY, int32 Radius, int32 X, int32 Y)
{
	return (FMath::Abs(X - CenterX) < Radius &&
			FMath::Abs(Y - CenterY) < Radius);
}

bool FVaFogRadiusStrategy_SquareStepped::IsInRadius(int32 CenterX, int32 CenterY, int32 Radius, int32 X, int32 Y)
{
	int32 HalfRadius = FMath::CeilToInt(static_cast<float>(Radius) / 2.f);
	return (FMath::Abs(X - CenterX) < HalfRadius &&
			FMath::Abs(Y - CenterY) < HalfRadius);
}
