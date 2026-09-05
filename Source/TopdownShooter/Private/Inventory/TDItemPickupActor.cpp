// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/TDItemPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Inventory/Data/TDItemDataAsset.h"
#include "Inventory/TDInventoryComponent.h"
#include "Character/TDPlayerCharacter.h"

ATDItemPickupActor::ATDItemPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(Root);
	MeshComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(Root);
	InteractionSphere->SetSphereRadius(150.f);
	InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void ATDItemPickupActor::BeginPlay()
{
	Super::BeginPlay();

	Count = FMath::Max(Count, 1);

	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ATDItemPickupActor::OnSphereBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ATDItemPickupActor::OnSphereEndOverlap);
}

#if WITH_EDITOR
void ATDItemPickupActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	Count = FMath::Max(Count, 1);
}
#endif

void ATDItemPickupActor::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ATDPlayerCharacter* Player = Cast<ATDPlayerCharacter>(OtherActor))
	{
		Player->SetFocusedInteractableActor(this);
	}
}

void ATDItemPickupActor::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ATDPlayerCharacter* Player = Cast<ATDPlayerCharacter>(OtherActor))
	{
		if (Player->GetFocusedInteractableActor() == this)
		{
			Player->SetFocusedInteractableActor(nullptr);
		}
	}
}

bool ATDItemPickupActor::TryPickup(ATDPlayerCharacter* Picker)
{
	bool bSuccess = false;

	if (Picker && ItemData)
	{
		if (UTDInventoryComponent* Inventory = Picker->GetInventoryComponent())
		{
			bSuccess = Inventory->AddItem(ItemData, Count);
		}
	}

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Warning, TEXT("[Pickup] TryPickup Item=%s Count=%d Result=%s"),
		ItemData ? *ItemData->ItemID.ToString() : TEXT("None"),
		Count,
		bSuccess ? TEXT("Success") : TEXT("Fail"));
#endif

	if (bSuccess)
	{
		if (Picker->GetFocusedInteractableActor() == this)
		{
			Picker->SetFocusedInteractableActor(nullptr);
		}
		Destroy();
	}

	return bSuccess;
}

bool ATDItemPickupActor::Interact_Implementation(ATDPlayerCharacter* Interactor)
{
	return TryPickup(Interactor);
}
