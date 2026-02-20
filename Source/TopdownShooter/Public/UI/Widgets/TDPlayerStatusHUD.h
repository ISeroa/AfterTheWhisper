// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDPlayerStatusHUD.generated.h"

class UProgressBar;

UCLASS()
class TOPDOWNSHOOTER_API UTDPlayerStatusHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetHealth(float Current, float Max);
	
protected:
	UPROPERTY(meta=(BindWidget))
	UProgressBar* ProgressBar_Health = nullptr;
};
