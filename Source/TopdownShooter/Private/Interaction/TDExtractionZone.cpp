// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/TDExtractionZone.h"
#include "Components/BoxComponent.h"
#include "Character/TDPlayerCharacter.h"

ATDExtractionZone::ATDExtractionZone()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	ExtractionArea = CreateDefaultSubobject<UBoxComponent>(TEXT("ExtractionArea"));
	ExtractionArea->SetupAttachment(Root);
	ExtractionArea->SetBoxExtent(FVector(150.f, 150.f, 100.f));
	ExtractionArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ExtractionArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	ExtractionArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ExtractionArea->SetGenerateOverlapEvents(true);
}

void ATDExtractionZone::BeginPlay()
{
	Super::BeginPlay();

	bIsExtractionActive = bStartActive;

	ExtractionArea->OnComponentBeginOverlap.AddDynamic(this, &ATDExtractionZone::OnExtractionAreaBeginOverlap);
	ExtractionArea->OnComponentEndOverlap.AddDynamic(this, &ATDExtractionZone::OnExtractionAreaEndOverlap);

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

	if (bIsPlayerInsideExtractionArea)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Extraction] Zone activated while player is inside"));
	}
#endif
}

bool ATDExtractionZone::IsExtractionActive() const
{
	return bIsExtractionActive;
}

bool ATDExtractionZone::IsPlayerInsideExtractionArea() const
{
	return bIsPlayerInsideExtractionArea;
}

void ATDExtractionZone::OnExtractionAreaBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATDPlayerCharacter* Player = Cast<ATDPlayerCharacter>(OtherActor);
	if (!Player) return;

	bIsPlayerInsideExtractionArea = true;

#if !UE_BUILD_SHIPPING
	if (bIsExtractionActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Extraction] Player entered active extraction area"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Extraction] Player entered inactive extraction area"));
	}
#endif
}

void ATDExtractionZone::OnExtractionAreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ATDPlayerCharacter* Player = Cast<ATDPlayerCharacter>(OtherActor);
	if (!Player) return;

	bIsPlayerInsideExtractionArea = false;

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Warning, TEXT("[Extraction] Player left extraction area"));
#endif
}
