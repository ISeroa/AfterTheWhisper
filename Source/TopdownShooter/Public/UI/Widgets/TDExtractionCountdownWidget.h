// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDExtractionCountdownWidget.generated.h"

class UTextBlock;

UCLASS()
class TOPDOWNSHOOTER_API UTDExtractionCountdownWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowCountdown(float RemainingTime);
	void UpdateCountdown(float RemainingTime);
	void HideCountdown();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Countdown = nullptr;
};
