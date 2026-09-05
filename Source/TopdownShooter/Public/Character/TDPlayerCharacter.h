// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TDBaseCharacter.h"
#include "TDPlayerCharacter.generated.h"

class UInputComponent;
class ATDWeaponBase;
class UTDWeaponPresetDA;
class UTDVisionComponent;
class UTDActorVisibilityComponent;
class UTDVisionRendererComponent;
class UTDInventoryComponent;
class UTDItemDataAsset;

UCLASS()
class TOPDOWNSHOOTER_API ATDPlayerCharacter : public ATDBaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATDPlayerCharacter();
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION(BlueprintPure, Category = "Weapon")
	ATDWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UTDInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetFocusedInteractableActor() const { return FocusedInteractableActor; }

	void SetFocusedInteractableActor(AActor* Interactable) { FocusedInteractableActor = Interactable; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void OnFirePressed();
	void OnFireReleased();

	void OnReloadPressed();

	void OnSprintPressed();
	void OnSprintReleased();

	void UpdateMoveSpeed();

	UFUNCTION(BlueprintCallable, Category = "Inventory|Test")
	void TestAddInventoryItem();

	void OnInteractPressed();

protected:
	//Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 600.f;

	// 인벤토리 총 무게로부터 계산된 결과값 (직접 조정 X, CalculateWeightSpeedMultiplier() 참고)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float WeightSpeedMultiplier = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bWantsToSprint = false;

	// 이 무게까지는 배율 패널티 없음 (1.0 유지)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Weight", meta = (ClampMin = "0.0"))
	float NoPenaltyWeight = 20.f;

	// 이 무게에서 MinWeightSpeedMultiplier에 도달
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Weight", meta = (ClampMin = "0.0"))
	float MaxPenaltyWeight = 40.f;

	// MaxPenaltyWeight 이상일 때 적용되는 최소 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Weight", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinWeightSpeedMultiplier = 0.7f;

protected:
	//Animation
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	float AimYaw;

	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	float AimPitch;

	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	bool bIsFiring;

	//Weapon
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ATDWeaponBase> DefaultWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UTDWeaponPresetDA* DefaultWeaponPreset = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category = "Weapon")
	ATDWeaponBase* CurrentWeapon = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UAnimMontage* FireMontage;

	//Aim
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aim")
	float AimInterpSpeed = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	TEnumAsByte<ECollisionChannel> AimTraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	float AimTraceDistance = 100000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	bool bFallbackToAimPlane = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim", meta = (ClampMin = "0.0"))
	float AimMaxDistance = 3000.f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim", meta = (ClampMin = "0.0"))
	float AimMinDistance = 30.f;

	// Stability (Aim Smoothing)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim|Stability")
	float TurnSpeedDegPerSec = 540.f;  

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim|Stability")
	float AimPointDeadZone = 2.0f;        

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim|Stability")
	float MinAbsDirZForPlane = 0.05f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aim|Stability")
	bool bHasLastAimPoint = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aim|Stability")
	FVector LastAimPoint = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aim|Stability")
	FVector SmoothedAimPoint = FVector::ZeroVector;

	//UI
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UTDW_AmmoWidget> AmmoWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UTDReloadBarWidget> ReloadBarWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UTDPlayerStatusHUD> StatusHUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UTDHitMarkerWidget> HitMarkerWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UTDCrosshairWidget> CrosshairWidgetClass;

	UPROPERTY()
	UTDW_AmmoWidget* AmmoWidget = nullptr;

	UPROPERTY()
	UTDReloadBarWidget* ReloadBarWidget = nullptr;

	UPROPERTY()
	UTDPlayerStatusHUD* StatusHUD = nullptr;

	UPROPERTY()
	UTDHitMarkerWidget* HitMarkerWidget = nullptr;

	UPROPERTY()
	UTDCrosshairWidget* CrosshairWidget = nullptr;

	//Vision
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vision")
	UTDVisionComponent* VisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vision")
	UTDActorVisibilityComponent* ActorVisibilityComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vision")
	UTDVisionRendererComponent* VisionRendererComponent = nullptr;

	//Inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UTDInventoryComponent* InventoryComponent = nullptr;

	// 테스트용 아이템 (BP에서 지정). TestAddInventoryItem으로 인벤토리에 추가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Test")
	UTDItemDataAsset* TestInventoryItem = nullptr;

	// 현재 상호작용(E) 범위 안에 있는 Interactable Actor. 없으면 nullptr
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	AActor* FocusedInteractableActor = nullptr;

	//Debug
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug|Aim")
	bool bDebugAimTrace = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug|Movement")
	bool bDebugMoveSpeed = false;

#if !UE_BUILD_SHIPPING
	float DebugCrosshairLogAccum = 0.f;
	float DebugMoveSpeedLogAccum = 0.f;
#endif



private:
	bool GetMouseAimPointRaw(FVector& OutAimPoint) const;

	UFUNCTION()
	void HandleHealthChanged(float NewHealth, float Delta);

	UFUNCTION()
	void HandleInventoryWeightChanged(float NewTotalWeight);

	float CalculateWeightSpeedMultiplier(float TotalWeight) const;

	UFUNCTION()
	void HandleWeaponFired();

	UFUNCTION()
	void HandleHitMarker();

	void TestDamage();
	void UpdateAimRotationFromPoint(float DeltaTime, const FVector& AimPoint);

	void Debug_PrintTraceChannel() const;
	void Debug_PrintHit(const FHitResult& Hit) const;
	void Debug_DrawTrace(const FVector& Start, const FVector& End, const FHitResult& Hit, bool bHit) const;
	void Debug_PrintMoveSpeed();
};
