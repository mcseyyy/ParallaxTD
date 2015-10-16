// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Engine/SceneCapture2D.h"
#include "Overseer.generated.h"

/**
 * 
 */
UCLASS()
class VENOM_API AOverseer : public APawn
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = Interface)
    void ShowMouse();
    UFUNCTION(BlueprintCallable, Category = Interface)
    void HideMouse();
};
