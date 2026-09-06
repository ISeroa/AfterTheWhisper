// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/TDInventoryComponent.h"

UTDInventoryComponent::UTDInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UTDInventoryComponent::CanAddItem(UTDItemDataAsset* Item, int32 Count) const
{
	if (!Item || Count <= 0) return false;

	const int32 StackMax = FMath::Max(Item->StackMax, 1);
	int32 Remaining = Count;

	if (StackMax > 1)
	{
		for (const FTDInventorySlot& Slot : Slots)
		{
			if (Slot.Item == Item && Slot.Count < StackMax)
			{
				Remaining -= (StackMax - Slot.Count);
				if (Remaining <= 0) return true;
			}
		}
	}

	const int32 SlotsNeeded = FMath::DivideAndRoundUp(Remaining, StackMax);
	return (Slots.Num() + SlotsNeeded) <= SlotCapacity;
}

bool UTDInventoryComponent::AddItem(UTDItemDataAsset* Item, int32 Count)
{
	if (!CanAddItem(Item, Count)) return false;

	const int32 StackMax = FMath::Max(Item->StackMax, 1);
	int32 Remaining = Count;

	if (StackMax > 1)
	{
		for (FTDInventorySlot& Slot : Slots)
		{
			if (Remaining <= 0) break;

			if (Slot.Item == Item && Slot.Count < StackMax)
			{
				const int32 AddAmount = FMath::Min(Remaining, StackMax - Slot.Count);
				Slot.Count += AddAmount;
				Remaining -= AddAmount;
			}
		}
	}

	while (Remaining > 0)
	{
		FTDInventorySlot NewSlot;
		NewSlot.Item = Item;
		NewSlot.Count = FMath::Min(Remaining, StackMax);
		Slots.Add(NewSlot);
		Remaining -= NewSlot.Count;
	}

	BroadcastInventoryChanged();
	return true;
}

bool UTDInventoryComponent::RemoveItem(UTDItemDataAsset* Item, int32 Count)
{
	if (!Item || Count <= 0) return false;

	int32 Available = 0;
	for (const FTDInventorySlot& Slot : Slots)
	{
		if (Slot.Item == Item) Available += Slot.Count;
	}
	if (Available < Count) return false;

	int32 Remaining = Count;
	for (int32 Index = Slots.Num() - 1; Index >= 0 && Remaining > 0; --Index)
	{
		FTDInventorySlot& Slot = Slots[Index];
		if (Slot.Item != Item) continue;

		const int32 RemoveAmount = FMath::Min(Remaining, Slot.Count);
		Slot.Count -= RemoveAmount;
		Remaining -= RemoveAmount;

		if (Slot.Count <= 0)
		{
			Slots.RemoveAt(Index);
		}
	}

	BroadcastInventoryChanged();
	return true;
}

float UTDInventoryComponent::GetTotalWeight() const
{
	float Total = 0.f;
	for (const FTDInventorySlot& Slot : Slots)
	{
		if (Slot.Item)
		{
			Total += Slot.Item->Weight * Slot.Count;
		}
	}
	return Total;
}

int32 UTDInventoryComponent::GetUsedSlotCount() const
{
	return Slots.Num();
}

bool UTDInventoryComponent::HasItem(UTDItemDataAsset* Item, int32 RequiredCount) const
{
	if (!Item || RequiredCount <= 0) return false;

	int32 Total = 0;
	for (const FTDInventorySlot& Slot : Slots)
	{
		if (Slot.Item == Item)
		{
			Total += Slot.Count;
			if (Total >= RequiredCount) return true;
		}
	}

	return false;
}

void UTDInventoryComponent::BroadcastInventoryChanged()
{
	OnInventoryChanged.Broadcast();
	OnInventoryWeightChanged.Broadcast(GetTotalWeight());
}
