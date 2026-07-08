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

protected:
	//Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 600.f;

	// 무게 시스템 도입 시 사용할 배율 (1.0f = 영향 없음)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WeightSpeedMultiplier = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bWantsToSprint = false;

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

	//Debug
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug|Aim")
	bool bDebugAimTrace = false;

#if !UE_BUILD_SHIPPING
	float DebugCrosshairLogAccum = 0.f;
#endif



private:
	bool GetMouseAimPointRaw(FVector& OutAimPoint) const;

	UFUNCTION()
	void HandleHealthChanged(float NewHealth, float Delta);

	UFUNCTION()
	void HandleWeaponFired();

	UFUNCTION()
	void HandleHitMarker();

	void TestDamage();
	void UpdateAimRotationFromPoint(float DeltaTime, const FVector& AimPoint);

	void Debug_PrintTraceChannel() const;
	void Debug_PrintHit(const FHitResult& Hit) const;
	void Debug_DrawTrace(const FVector& Start, const FVector& End, const FHitResult& Hit, bool bHit) const;
};
