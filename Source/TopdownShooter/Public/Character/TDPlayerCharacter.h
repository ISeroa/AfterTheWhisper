// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TDPlayerCharacter.generated.h"

class UInputComponent;
class ATDWeaponBase;
class UTDWeaponPresetDA;

UCLASS()
class TOPDOWNSHOOTER_API ATDPlayerCharacter : public ACharacter
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

	UPROPERTY()
	UTDW_AmmoWidget* AmmoWidget;

	//Debug
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug|Aim")
	bool bDebugAimTrace = false;



private:
	bool GetMouseAimPointRaw(FVector& OutAimPoint) const;
	void UpdateAimRotationFromPoint(float DeltaTime, const FVector& AimPoint);

	void Debug_PrintTraceChannel() const;
	void Debug_PrintHit(const FHitResult& Hit) const;
	void Debug_DrawTrace(const FVector& Start, const FVector& End, const FHitResult& Hit, bool bHit) const;
};
