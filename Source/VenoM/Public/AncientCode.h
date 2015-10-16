// Copyright VenoM Inc. 2014-2015

#pragma once

#include "GameFramework/Actor.h"
#include "AncientCode.generated.h"

UCLASS()
class VENOM_API AAncientCode : public AActor
{
	GENERATED_BODY()
	
public:	
	AAncientCode(const FObjectInitializer& ObjectInitializer);
	//AAncientCode();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene Component")
	USceneComponent* SceneComponent; // serves as the rootof the object

	UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = Fire)
	TArray<UParticleSystemComponent*> fire;



	UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = Fire)
	UParticleSystemComponent *fire1;
	
	
};
