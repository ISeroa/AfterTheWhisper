// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/TDExtractionActivator.h"
#include "Components/SphereComponent.h"
#include "Inventory/Data/TDItemDataAsset.h"
#include "Inventory/TDInventoryComponent.h"
#include "Character/TDPlayerCharacter.h"
#include "Interaction/TDExtractionZone.h"

ATDExtractionActivator::ATDExtractionActivator()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(Root);
	InteractionSphere->SetSphereRadius(150.f);
	InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void ATDExtractionActivator::BeginPlay()
{
	Super::BeginPlay();

	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ATDExtractionActivator::OnSphereBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ATDExtractionActivator::OnSphereEndOverlap);
}

void ATDExtractionActivator::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ATDPlayerCharacter* Player = Cast<ATDPlayerCharacter>(OtherActor))
	{
		Player->SetFocusedInteractableActor(this);
	}
}

void ATDExtractionActivator::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ATDPlayerCharacter* Player = Cast<ATDPlayerCharacter>(OtherActor))
	{
		if (Player->GetFocusedInteractableActor() == this)
		{
			Player->SetFocusedInteractableActor(nullptr);
		}
	}
}

bool ATDExtractionActivator::Interact_Implementation(ATDPlayerCharacter* Interactor)
{
	if (!Interactor) return false;

	if (!TargetExtractionZone)
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("[ExtractionActivator] Invalid target extraction zone"));
#endif
		return false;
	}

	if (TargetExtractionZone->IsExtractionActive())
	{
		return true;
	}

	if (!RequiredItem)
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("[ExtractionActivator] Required item is not configured"));
#endif
		return false;
	}

	UTDInventoryComponent* Inventory = Interactor->GetInventoryComponent();
	if (!Inventory) return false;

	const bool bHasItem = Inventory->HasItem(RequiredItem, 1);

	if (!bHasItem)
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("[ExtractionActivator] Required item missing: %s"), *RequiredItem->ItemID.ToString());
#endif
		return false;
	}

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Warning, TEXT("[ExtractionActivator] Activation condition passed: %s"), *RequiredItem->ItemID.ToString());
#endif

	TargetExtractionZone->ActivateExtraction();
	return true;
}
