// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Math/Transform.h"
#include "Sound/SoundBase.h"
#include "NiagaraSystem.h"
#include "Weapon/Data/TDWeaponPartDA.h"
#include "TDWeaponPresetDA.generated.h"

class ATDCasing;

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

USTRUCT(BlueprintType)
struct FTDWeaponStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon|Fire")
	float FireRate = 12.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon|Fire")
	float Range = 100000.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon|Fire")
	float Damage = 10.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon|Fire")
	float SpreadDeg = 0.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon|Fire")
	bool bIsAutomatic = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon|Ammo")
	int32 MagazineSize = 12;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon|Reload")
	float ReloadTime = 1.4f;
};

USTRUCT(BlueprintType)
struct FWeaponSoundSet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* FireIndoor = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* FireOutdoor = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* DryFire = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* CasingDrop = nullptr;
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
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FTDWeaponStats Stats;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FWeaponSoundSet SoundSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UNiagaraSystem* MuzzleFlashEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Assemble")
	FName MuzzleSocketName = "SCK_Muzzle";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Casing")
	TSubclassOf<ATDCasing> CasingClass;
};

