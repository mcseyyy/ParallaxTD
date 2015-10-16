#include "VenoM.h"
#include "VenoMMath.h"

FBox2D FRect2D::GetBoundingBox()
{
    FVector2D Min = Centre,
              Max = Centre;
    FVector2D PosExtentX = GetExtentX();
    FVector2D PosExtentY = GetExtentY();
    if (PosExtentX.X < 0) PosExtentX.X *= -1;
    if (PosExtentX.Y < 0) PosExtentX.Y *= -1;
    if (PosExtentY.X < 0) PosExtentY.X *= -1;
    if (PosExtentY.Y < 0) PosExtentY.Y *= -1;
    Max += (PosExtentX + PosExtentY);
    Min -= (PosExtentX + PosExtentY);
    return FBox2D(Min, Max);
}

bool Rect2DContains(const FRect2D Outer, const FRect2D Inner){
    FBox2D InnerBounds = NormalizeToRect(Outer, Inner).GetBoundingBox();
    if (InnerBounds.Min.X > 0.f && InnerBounds.Min.Y > 0.f &&
        InnerBounds.Max.X < 1.f && InnerBounds.Max.Y < 1.f) return true;
    return false;
};
bool Rect2DIntersect(const FRect2D A, const FRect2D B){
    FBox2D NormalBoundB = NormalizeToRect(A, B).GetBoundingBox();
    if (NormalBoundB.Min.X > 1.f) return false;
    if (NormalBoundB.Min.Y > 1.f) return false;
    if (NormalBoundB.Max.X < 0.f) return false;
    if (NormalBoundB.Max.Y < 0.f) return false;
    FBox2D NormalBoundA = NormalizeToRect(B, A).GetBoundingBox();
    if (NormalBoundA.Min.X > 1.f) return false;
    if (NormalBoundA.Min.Y > 1.f) return false;
    if (NormalBoundA.Max.X < 0.f) return false;
    if (NormalBoundA.Max.Y < 0.f) return false;
    return true;
};

// Transforms BaseRect into the coordinate system where NormalRect is an AABB
// from (0,0) to (1,1). This is defined as:
//   The origin (0,0) is located at the bottom left corner of NormalRect
//   The axes X,Y are the positive X,Y according to the rotation of NormalRect
//   The X,Y unit lengths are equal to the X and Y lengths of NormalRect
FRect2D NormalizeToRect(FRect2D NormalRect, FRect2D BaseRect)
{
    FRect2D Result;
    Result.Centre = BaseRect.Centre - NormalRect.Centre;
    Result.Centre = RotateVector2D(Result.Centre, -NormalRect.Rotation);
    Result.Centre /= NormalRect.Size;
    Result.Centre += FVector2D(0.5,0.5);
    Result.Size = BaseRect.Size / NormalRect.Size;
    Result.Rotation = BaseRect.Rotation - NormalRect.Rotation;
    return Result;
}
// The inverse of NormalizeToRect
// If a Rect is normalized onto one Rect and scaled by another with a different
// X:Y ratio, that Rect will be squashed/stretched along its own X,Y axes,
// not those of the normal or scale Rects
FRect2D ScaleToRect(FRect2D ScaleRect, FRect2D BaseRect)
{
    FRect2D Result;
    Result.Centre = BaseRect.Centre - FVector2D(0.5,0.5);
    Result.Centre = Result.Centre * ScaleRect.Size;
    Result.Centre = RotateVector2D(Result.Centre, ScaleRect.Rotation);
    Result.Centre += ScaleRect.Centre;
    Result.Size = BaseRect.Size * ScaleRect.Size;
    Result.Rotation = BaseRect.Rotation + ScaleRect.Rotation;
    return Result;
}
// Normalizes BaseRect to NormalRect, then scales the result to ScaleRect
FRect2D RescaleRect(FRect2D NormalRect, FRect2D ScaleRect, FRect2D BaseRect)
{
    return ScaleToRect(ScaleRect, NormalizeToRect(NormalRect, BaseRect));
}
 
FVector2D RotateVector2D(FVector2D V2, degrees angle)
{
    auto Vec3 = FVector(V2, 0);
    Vec3 = Vec3.RotateAngleAxis(angle, FVector(0.f, 0.f, 1.f));
    return FVector2D(Vec3.X, Vec3.Y);
}