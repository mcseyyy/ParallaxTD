// Copyright VenoM Inc. 2014-2015

#pragma once

#include "VenoM.h"
#include "VenoMMath.generated.h"

typedef float degrees;

FVector2D RotateVector2D(FVector2D V2, degrees angle);

USTRUCT(BlueprintType)
struct FRect2D{
    GENERATED_USTRUCT_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry")
    FVector2D Centre;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry")
    FVector2D Size;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry")
    float Rotation;
    FVector2D GetExtentX() const{
        return RotateVector2D(FVector2D(Size.X/2, 0.f), Rotation);
    }
    FVector2D GetExtentY() const{
        return RotateVector2D(FVector2D(0.f, Size.Y/2), Rotation);
    }
    FBox2D GetBoundingBox();
};

bool Rect2DContains(const FRect2D Outer, const FRect2D Inner);
bool Rect2DIntersect(const FRect2D A, const FRect2D B);

FRect2D NormalizeToRect(FRect2D NormalRect, FRect2D BaseRect);
FRect2D ScaleToRect(FRect2D ScaleRect, FRect2D BaseRect);
FRect2D RescaleRect(FRect2D NormalRect, FRect2D ScaleRect, FRect2D BaseRect);

