// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/TDW_AmmoWidget.h"
#include "Weapon/TDWeaponBase.h"
#include "Components/TextBlock.h"

void UTDW_AmmoWidget::BindWeapon(ATDWeaponBase* InWeapon)
{
    if (BoundWeapon)
    {
        BoundWeapon->OnAmmoChanged.RemoveDynamic(this, &UTDW_AmmoWidget::HandleAmmoChanged);
    }

    BoundWeapon = InWeapon;

    if (BoundWeapon)
    {
        BoundWeapon->OnAmmoChanged.AddDynamic(this, &UTDW_AmmoWidget::HandleAmmoChanged);

        BP_UpdateAmmo(BoundWeapon->GetAmmoInMag(), BoundWeapon->GetMagazineSize(), 100.f);
    }
}

void UTDW_AmmoWidget::HandleAmmoChanged(int32 InAmmoInMag, int32 InMagazineSize)
{
    BP_UpdateAmmo(InAmmoInMag, InMagazineSize, 100.f);
}