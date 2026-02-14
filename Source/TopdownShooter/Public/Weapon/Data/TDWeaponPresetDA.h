// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Math/Transform.h"
#include "Weapon/Types/TDWeaponTypes.h"
#include "Weapon/Data/TDWeaponPartDA.h"
#include "TDWeaponPresetDA.generated.h"

USTRUCT(BlueprintType)
struct FTDGripOffset
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName HandSocketName = "hand_r_socket";

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FTransform RelativeToHand = FTransform::Identity;
};

USTRUCT(BlueprintType)
struct FTDWeaponPartEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UTDWeaponPartDA* Part = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bOverrideOffset = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (EditCondition = "bOverrideOffset"))
	FTransform OverrideRelativeToSocket = FTransform::Identity;
};

UCLASS()
class TOPDOWNSHOOTER_API UTDWeaponPresetDA : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMesh* FrameMesh = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FTDGripOffset Grip;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FTDWeaponPartEntry> Parts;
};

