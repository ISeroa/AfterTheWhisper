// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDExtractionZone.generated.h"

class USceneComponent;

UCLASS()
class TOPDOWNSHOOTER_API ATDExtractionZone : public AActor
{
	GENERATED_BODY()

public:
	ATDExtractionZone();

	UFUNCTION(BlueprintCallable, Category = "Extraction")
	void ActivateExtraction();

	UFUNCTION(BlueprintPure, Category = "Extraction")
	bool IsExtractionActive() const;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Extraction")
	void OnExtractionActivated();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extraction")
	USceneComponent* Root = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Extraction")
	bool bStartActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extraction")
	bool bIsExtractionActive = false;
};
