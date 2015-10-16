// Fill out your copyright notice in the Description page of Project Settings.

#include "VenoM.h"
#include "Overseer.h"


void AOverseer::ShowMouse()
{
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController){
        PlayerController->bEnableClickEvents = true;
        PlayerController->bShowMouseCursor = true;
        PlayerController->bEnableMouseOverEvents = true;
    }
}

void AOverseer::HideMouse()
{
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController){
        PlayerController->bEnableClickEvents = false;
        PlayerController->bShowMouseCursor = false;
        PlayerController->bEnableMouseOverEvents = false;
    }
}


