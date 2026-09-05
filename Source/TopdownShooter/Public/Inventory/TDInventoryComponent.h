// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Data/TDItemDataAsset.h"
#include "TDInventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FTDInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	UTDItemDataAsset* Item = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 Count = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTDOnInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTDOnInventoryWeightChanged, float, NewTotalWeight);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TOPDOWNSHOOTER_API UTDInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTDInventoryComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 SlotCapacity = 20;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<FTDInventorySlot> Slots;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FTDOnInventoryChanged OnInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FTDOnInventoryWeightChanged OnInventoryWeightChanged;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool CanAddItem(UTDItemDataAsset* Item, int32 Count) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UTDItemDataAsset* Item, int32 Count);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(UTDItemDataAsset* Item, int32 Count);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	float GetTotalWeight() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetUsedSlotCount() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(UTDItemDataAsset* Item, int32 RequiredCount = 1) const;

private:
	void BroadcastInventoryChanged();
};
