// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/TDReloadBarWidget.h"
#include "Weapon/TDWeaponBase.h"

void UTDReloadBarWidget::BindWeapon(ATDWeaponBase* InWeapon)
{
	if (!InWeapon)
	{
		return;
	}

	BoundWeapon = InWeapon;
	
	BoundWeapon->OnReloadUIStart.AddDynamic(this, &UTDReloadBarWidget::HandleReloadUIStart);
	BoundWeapon->OnReloadUIStop.AddDynamic(this, &UTDReloadBarWidget::HandleReloadUIStop);

	HandleReloadUIStop();
}

void UTDReloadBarWidget::HandleReloadUIStart(float Duration)
{
	BP_ShowReloadBar(Duration);
}

void UTDReloadBarWidget::HandleReloadUIStop()
{
	BP_HideReloadBar();
}
