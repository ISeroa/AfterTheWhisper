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
	UFUNCTION(BlueprintCallable)
	void BindWeapon(ATDWeaponBase* InWeapon);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateAmmo(int32 InCurrentAmmo, int32 InMagSize, float InMaxDisplay);

	UFUNCTION()
		void HandleAmmoChanged(int32 InAmmoInMag, int32 InMagazineSize);

protected:

private:
	int32 CachedAmmo = -1;
	int32 CachedMagSize = -1;

	UPROPERTY()
	ATDWeaponBase* BoundWeapon = nullptr;
};
