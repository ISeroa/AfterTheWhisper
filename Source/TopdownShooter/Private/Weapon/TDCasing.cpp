#include "Weapon/TDCasing.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ATDCasing::ATDCasing()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

	MeshComp->BodyInstance.bSimulatePhysics = true;
	MeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));
}

void ATDCasing::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(8.f);

	MeshComp->SetNotifyRigidBodyCollision(true);
	MeshComp->OnComponentHit.AddDynamic(this, &ATDCasing::OnHit);
}

void ATDCasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bPlayedImpact) return;
	if (!OtherComp || OtherComp->GetCollisionObjectType() != ECC_WorldStatic) return;

	const float Speed = MeshComp->GetPhysicsLinearVelocity().Size();
	if (Speed < MinImpactSpeed) return;

	bPlayedImpact = true;

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Hit.ImpactPoint);
	}
}
