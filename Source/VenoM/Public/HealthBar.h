// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

/**
 * 
 */
UCLASS()
class VENOM_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
	float hp;
	UFUNCTION(BlueprintCallable, Category = "HealthBar")
	void AddToCreep();
	
};
