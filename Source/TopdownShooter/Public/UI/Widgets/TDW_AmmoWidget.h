// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDW_AmmoWidget.generated.h"

class ATDWeaponBase;

UCLASS()
class TOPDOWNSHOOTER_API UTDW_AmmoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindWeapon(ATDWeaponBase* Weapon);

protected:
	UFUNCTION()
	void HandleAmmoChanged(int32 InAmmo, int32 InMagSize);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Ammo;

	UPROPERTY()
	ATDWeaponBase* BoundWeapon;

private:
	int32 CachedAmmo = -1;
	int32 CachedMagSize = -1;
};
