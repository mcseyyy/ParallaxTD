// Copyright VenoM Inc. 2014-2015

#include "VenoM.h"
#include "PlayerZone.h"


void APlayerZone::SetBounds(FVector2D TopRight, FVector2D BottomLeft, float Rotation)
{
    Bounds.Centre = (BottomLeft + TopRight)/2;
    Bounds.Size = TopRight - BottomLeft;
    Bounds.Rotation = Rotation;
}
