#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDWeaponBase.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UTDWeaponPresetDA;

USTRUCT(BlueprintType)
struct FWeaponPartSpec
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName Slot = NAME_None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMesh* Mesh = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName AttachSocket = NAME_None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform RelativeTransform = FTransform::Identity;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, AmmoInMag, int32, MagazineSize);

UCLASS()
class TOPDOWNSHOOTER_API ATDWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ATDWeaponBase();

	void SetTriggerHeld(bool bHeld);
	void SetAimTarget(const FVector& InAimTarget) { AimTarget = InAimTarget; }

	int32 GetAmmoInMag()  const { return AmmoInMag; }
	int32 GetMagazineSize() const { return MagazineSize; }

	UFUNCTION(BlueprintCallable, Category = "Weapon|Assemble")
	FName GetHandSocketName() const { return HandSocketName; }

	UFUNCTION(BlueprintCallable, Category = "Weapon|Assemble")
	void SetPartsFromPreset(UTDWeaponPresetDA* Preset, bool bClearMissingSlot = true);
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Assemble")
	void SetParts(const TArray <FWeaponPartSpec>& InParts);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Assemble")
	void SetPart(const  FWeaponPartSpec& InPart);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Assemble")
	void ClearPart(FName Slot);

	void RequestReload();

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnAmmoChanged OnAmmoChanged;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void FireOnce();
	bool CanFire() const;
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void StartReload();
	void FinishReload();

	void StartFireLoop();
	void StopFireLoop();

	FVector GetMuzzleLocation() const;
	FVector GetShotDirection() const;

	void NotifyAmmoChanged();

	void ApplyParts();
	void ClearAllPartsNotIn(const TSet<FName>& KeepSlots);
	UStaticMeshComponent* GetOrCreatePartComp(FName Slot);
	UStaticMeshComponent* FindMuzzleProviderComp() const;

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* GripRoot;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	//weapon parts
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Assemble")
	FName MuzzlePrioritySlot = "Barrel";

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Assemble")
	FName MuzzleSocketName = "Muzzle";

	//weapon options
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Fire")
	float FireRate = 12.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Fire")
	float Range = 100000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Fire")
	float Damage = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Fire")
	float SpreadDeg = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Ammo")
	int32 MagazineSize = 12;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Ammo")
	float ReloadTIme = 1.4f;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 AmmoInMag = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 AmmoReserve = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon|Ammo")
	bool bReloading = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Debug")
	bool bDebugShot = false;

private:
	UPROPERTY(Transient)
	UTDWeaponPresetDA* CurrentPreset = nullptr;

	UPROPERTY(Transient)
	TMap<FName, UStaticMeshComponent*> PartComps;

	UPROPERTY(Transient)
	TMap<FName, FWeaponPartSpec> Parts;

	UPROPERTY(Transient)
		FName HandSocketName = "hand_r_socket";

	FTimerHandle Timerhandle_FireLoop;
	FTimerHandle Timerhandle_Reload;
	bool bTriggerHeld = false;
	FVector AimTarget = FVector::ZeroVector;
};
