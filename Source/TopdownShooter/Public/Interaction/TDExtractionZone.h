// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/TDInteractableInterface.h"
#include "TDExtractionZone.generated.h"

class USceneComponent;
class USphereComponent;
class UTDItemDataAsset;
class ATDPlayerCharacter;

UCLASS()
class TOPDOWNSHOOTER_API ATDExtractionZone : public AActor, public ITDInteractableInterface
{
	GENERATED_BODY()

public:
	ATDExtractionZone();

	virtual bool Interact_Implementation(ATDPlayerCharacter* Interactor) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extraction")
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extraction")
	USphereComponent* InteractionSphere = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Extraction")
	UTDItemDataAsset* RequiredItem = nullptr;
};
