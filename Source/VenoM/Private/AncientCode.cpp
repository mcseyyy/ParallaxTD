// Copyright VenoM Inc. 2014-2015

#include "VenoM.h"
#include "AncientCode.h"


// Sets default values
AAncientCode::AAncientCode(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Scene Component"));
	RootComponent = SceneComponent;
	fire1 = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("Fire single"));
	fire.Add(ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("fire0")));
	fire.Add(ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("fire1")));
	//for (int32 c = 0; c < 10;c++)
	//{
	//	fire[c] = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("fire %d",c));
	//}


}

// Called when the game starts or when spawned
void AAncientCode::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAncientCode::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

