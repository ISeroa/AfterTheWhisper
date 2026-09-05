// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TDInteractableInterface.generated.h"

class ATDPlayerCharacter;

UINTERFACE(Blueprintable)
class TOPDOWNSHOOTER_API UTDInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class TOPDOWNSHOOTER_API ITDInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	bool Interact(ATDPlayerCharacter* Interactor);
};
