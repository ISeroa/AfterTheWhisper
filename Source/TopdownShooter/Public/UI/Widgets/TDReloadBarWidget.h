// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDReloadBarWidget.generated.h"

class ATDWeaponBase;

UCLASS()
class TOPDOWNSHOOTER_API UTDReloadBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void BindWeapon(ATDWeaponBase* InWeapon);

protected:
	UFUNCTION()
	void HandleReloadUIStart(float Duration);

	UFUNCTION()
	void HandleReloadUIStop();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void BP_ShowReloadBar(float Duration);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void BP_HideReloadBar();

private:
	UPROPERTY(Transient)
	ATDWeaponBase* BoundWeapon = nullptr;
};
