#include "Weapon/TDCasing.h"
#include "Components/StaticMeshComponent.h"

ATDCasing::ATDCasing()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

	MeshComp->SetSimulatePhysics(true);        
	MeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));
}

void ATDCasing::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(8.f);
}
