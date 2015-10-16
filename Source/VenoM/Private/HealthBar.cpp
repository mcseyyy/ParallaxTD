// Fill out your copyright notice in the Description page of Project Settings.

#include "VenoM.h"
#include "BasicCreep.h"
#include "HealthBar.h"


void UHealthBar::AddToCreep()
{
	if (!GetIsVisible())
	{
		AddToViewport();
	}
	
	ABasicCreep *creep = Cast<ABasicCreep>(GetOwningPlayerPawn());
	if (creep)
	{
		hp = (float)creep->GetHP() / (float)creep->GetHP();

	}
}
