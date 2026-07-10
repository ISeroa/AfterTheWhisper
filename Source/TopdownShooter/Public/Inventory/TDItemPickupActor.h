// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDItemPickupActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USphereComponent;
class UTDItemDataAsset;
class ATDPlayerCharacter;

UCLASS()
class TOPDOWNSHOOTER_API ATDItemPickupActor : public AActor
{
	GENERATED_BODY()

public:
	ATDItemPickupActor();

	UFUNCTION(BlueprintPure, Category = "Pickup")
	UTDItemDataAsset* GetItemData() const { return ItemData; }

	UFUNCTION(BlueprintPure, Category = "Pickup")
	int32 GetCount() const { return Count; }

	// Picker의 InventoryComponent에 AddItem을 시도한다. 성공 시 이 액터는 Destroy된다.
	bool TryPickup(ATDPlayerCharacter* Picker);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	UStaticMeshComponent* MeshComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	USphereComponent* InteractionSphere = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	UTDItemDataAsset* ItemData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = "1"))
	int32 Count = 1;
};
