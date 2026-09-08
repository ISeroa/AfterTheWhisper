// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/TDNavigationIndicatorWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UTDNavigationIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HideIndicator();
}

void UTDNavigationIndicatorWidget::ShowIndicator()
{
	StopAllAnimations();

	SetVisibility(ESlateVisibility::HitTestInvisible);
	SetRenderOpacity(1.0f);

	if (HorizontalBox_Indicator)
	{
		HorizontalBox_Indicator->SetVisibility(ESlateVisibility::HitTestInvisible);
		HorizontalBox_Indicator->SetRenderOpacity(1.0f);
	}

	const UEnum* VisibilityEnum = StaticEnum<ESlateVisibility>();

	const FString SelfVisibilityStr = VisibilityEnum
		? VisibilityEnum->GetNameStringByValue((int64)GetVisibility())
		: TEXT("UNKNOWN");
	const FString BoxVisibilityStr = HorizontalBox_Indicator
		? (VisibilityEnum ? VisibilityEnum->GetNameStringByValue((int64)HorizontalBox_Indicator->GetVisibility()) : TEXT("UNKNOWN"))
		: TEXT("NULL");
	const float BoxOpacity = HorizontalBox_Indicator ? HorizontalBox_Indicator->GetRenderOpacity() : -1.0f;

	UE_LOG(LogTemp, Warning,
		TEXT("[Navigation] Show: Self=%s, Box=%s, SelfOpacity=%.2f, BoxOpacity=%.2f"),
		*SelfVisibilityStr, *BoxVisibilityStr, GetRenderOpacity(), BoxOpacity);
}

void UTDNavigationIndicatorWidget::UpdateIndicator(const FVector2D& ScreenDirection, float DistanceMeters)
{
	if (HorizontalBox_Indicator)
	{
		HorizontalBox_Indicator->SetRenderTranslation(ScreenDirection * IndicatorRadius);
	}

	if (Text_Arc)
	{
		const float AngleDeg = FMath::RadiansToDegrees(FMath::Atan2(ScreenDirection.Y, ScreenDirection.X));
		Text_Arc->SetRenderTransformAngle(AngleDeg);
	}

	if (Text_Distance)
	{
		const int32 RoundedMeters = FMath::RoundToInt(DistanceMeters);
		Text_Distance->SetText(FText::FromString(FString::Printf(TEXT("%dm"), RoundedMeters)));
	}
}

void UTDNavigationIndicatorWidget::BeginFadeOut()
{
	if (FadeOut)
	{
		PlayAnimation(FadeOut);
	}
}

void UTDNavigationIndicatorWidget::HideIndicator()
{
	if (FadeOut)
	{
		StopAnimation(FadeOut);
	}

	if (HorizontalBox_Indicator)
	{
		HorizontalBox_Indicator->SetVisibility(ESlateVisibility::Collapsed);
	}
}
