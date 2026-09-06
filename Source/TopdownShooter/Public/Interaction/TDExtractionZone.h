// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDExtractionZone.generated.h"

class USceneComponent;
class UBoxComponent;
class ATDPlayerCharacter;

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

	UFUNCTION(BlueprintPure, Category = "Extraction")
	bool IsPlayerInsideExtractionArea() const;

	UFUNCTION(BlueprintPure, Category = "Extraction")
	bool IsExtractionCompleted() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Extraction")
	void OnExtractionActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Extraction")
	void OnExtractionCompleted();

	UFUNCTION()
	void OnExtractionAreaBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnExtractionAreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void StartExtractionTimer();
	void CancelExtractionTimer();
	void CompleteExtraction();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extraction")
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extraction")
	UBoxComponent* ExtractionArea = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Extraction")
	bool bStartActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extraction")
	bool bIsExtractionActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extraction")
	bool bIsPlayerInsideExtractionArea = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Extraction", meta = (ClampMin = "0.1"))
	float ExtractionDuration = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extraction")
	bool bIsExtractionCompleted = false;

	FTimerHandle ExtractionTimerHandle;
};
