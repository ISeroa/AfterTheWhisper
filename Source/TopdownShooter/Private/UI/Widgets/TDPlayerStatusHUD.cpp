// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/TDPlayerStatusHUD.h"
#include "Components/ProgressBar.h"

void UTDPlayerStatusHUD::SetHealth(float Current, float Max)
{
	const float Percent = Max > 0.f ? Current / Max : 0.f;
	if (ProgressBar_Health)
	{
		ProgressBar_Health->SetPercent(Percent);
	}
}
