// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/TDExtractionZone.h"

ATDExtractionZone::ATDExtractionZone()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void ATDExtractionZone::BeginPlay()
{
	Super::BeginPlay();

	bIsExtractionActive = bStartActive;

	if (bIsExtractionActive)
	{
		OnExtractionActivated();
	}
}

void ATDExtractionZone::ActivateExtraction()
{
	if (bIsExtractionActive) return;

	bIsExtractionActive = true;
	OnExtractionActivated();

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Warning, TEXT("[Extraction] Extraction zone activated"));
#endif
}

bool ATDExtractionZone::IsExtractionActive() const
{
	return bIsExtractionActive;
}
