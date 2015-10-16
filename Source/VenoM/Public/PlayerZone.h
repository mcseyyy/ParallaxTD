// Copyright VenoM Inc. 2014-2015

#pragma once

#include "GameFramework/Actor.h"
#include "VisionDeviceReceiver.h"
#include "VenoMMath.h"
#include "PlayerZone.generated.h"

UCLASS()
class VENOM_API APlayerZone : public AActor
{
	GENERATED_BODY()
public:	
    UFUNCTION(BlueprintCallable, Category = Info)
    void SetBounds(FVector2D TopRight, FVector2D BottomLeft, float Rotation);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info)
    int32 TeamID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info)
    int32 SlotID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info)
    FRect2D Bounds;
};
