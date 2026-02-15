// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/TDW_AmmoWidget.h"
#include "Weapon/TDWeaponBase.h"
#include "Components/TextBlock.h"

void UTDW_AmmoWidget::BindWeapon(ATDWeaponBase* Weapon)
{
	BoundWeapon = Weapon;

	if (BoundWeapon)
	{
		BoundWeapon->OnAmmoChanged.AddDynamic(this, &UTDW_AmmoWidget::HandleAmmoChanged);

		HandleAmmoChanged(BoundWeapon->GetAmmoInMag(), BoundWeapon->GetMagazineSize());

	}
}

void UTDW_AmmoWidget::HandleAmmoChanged(int32 InAmmo, int32 InMagSize)
{
	if (InAmmo == CachedAmmo && InMagSize == CachedMagSize)
	{
		return;
	}

	CachedAmmo = InAmmo;
	CachedMagSize = InMagSize;

	if (Text_Ammo)
	{
		Text_Ammo->SetText(
			FText::FromString(FString::Printf(TEXT("%d / %d"), InAmmo, InMagSize))
		);
	}
}