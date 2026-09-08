// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDNavigationIndicatorWidget.generated.h"

class UHorizontalBox;
class UTextBlock;
class UWidgetAnimation;

UCLASS()
class TOPDOWNSHOOTER_API UTDNavigationIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowIndicator();
	void UpdateIndicator(const FVector2D& ScreenDirection, float DistanceMeters);
	void BeginFadeOut();
	void HideIndicator();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HorizontalBox_Indicator = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Arc = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Distance = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOut = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Navigation")
	float IndicatorRadius = 120.f;
};
