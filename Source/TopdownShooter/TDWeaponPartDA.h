// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TDWeaponTypes.h"
#include "TDWeaponPartDA.generated.h"


UCLASS()
class TOPDOWNSHOOTER_API UTDWeaponPartDA : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETDWeaponSlot Slot = ETDWeaponSlot::Slide;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName AttachSocketName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform RelativeOffset = FTransform::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDisableCollision = true;
};
