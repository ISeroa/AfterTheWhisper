// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/TDExtractionZone.h"
#include "Components/SphereComponent.h"
#include "Inventory/Data/TDItemDataAsset.h"
#include "Inventory/TDInventoryComponent.h"
#include "Character/TDPlayerCharacter.h"

ATDExtractionZone::ATDExtractionZone()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(Root);
	InteractionSphere->SetSphereRadius(150.f);
	InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void ATDExtractionZone::BeginPlay()
{
	Super::BeginPlay();

	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ATDExtractionZone::OnSphereBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ATDExtractionZone::OnSphereEndOverlap);
}

void ATDExtractionZone::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ATDPlayerCharacter* Player = Cast<ATDPlayerCharacter>(OtherActor))
	{
		Player->SetFocusedInteractableActor(this);
	}
}

void ATDExtractionZone::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ATDPlayerCharacter* Player = Cast<ATDPlayerCharacter>(OtherActor))
	{
		if (Player->GetFocusedInteractableActor() == this)
		{
			Player->SetFocusedInteractableActor(nullptr);
		}
	}
}

bool ATDExtractionZone::Interact_Implementation(ATDPlayerCharacter* Interactor)
{
	if (!Interactor || !RequiredItem) return false;

	UTDInventoryComponent* Inventory = Interactor->GetInventoryComponent();
	if (!Inventory) return false;

	const bool bHasItem = Inventory->HasItem(RequiredItem, 1);

#if !UE_BUILD_SHIPPING
	if (bHasItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Extraction] Success: %s"), *RequiredItem->ItemID.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Extraction] Required item missing: %s"), *RequiredItem->ItemID.ToString());
	}
#endif

	return bHasItem;
}
