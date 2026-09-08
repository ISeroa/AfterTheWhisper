// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/TDExtractionCountdownWidget.h"
#include "Components/TextBlock.h"

void UTDExtractionCountdownWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HideCountdown();
}

void UTDExtractionCountdownWidget::ShowCountdown(float RemainingTime)
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	UpdateCountdown(RemainingTime);
}

void UTDExtractionCountdownWidget::UpdateCountdown(float RemainingTime)
{
	const float ClampedTime = FMath::Max(RemainingTime, 0.0f);

	if (Text_Countdown)
	{
		Text_Countdown->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), ClampedTime)));
	}
}

void UTDExtractionCountdownWidget::HideCountdown()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
