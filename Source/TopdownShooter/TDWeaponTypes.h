#pragma once

#include "CoreMinimal.h"
#include "TDWeaponTypes.generated.h"

UENUM(BlueprintType)
enum class ETDWeaponSlot : uint8
{
	Frame				UMETA(DisplayName = "Frame"),
	Slide				UMETA(DisplayName = "Slide"),
	Hammer			UMETA(DisplayName = "Hammer"),
	Magazine		UMETA(DisplayName = "Magazine"),
	Barrel				UMETA(DisplayName = "Barrel"),
	Muzzle			UMETA(DisplayName = "Muzzle"),
	Sight				UMETA(DisplayName = "Sight"),
	Grip					UMETA(DisplayName = "Grip"),
	Trigger			UMETA(DisplayName = "Trigger"),
	Ejector			UMETA(DisplayName = "Ejector")

};

FORCEINLINE FName TDSlotToName(ETDWeaponSlot Slot)
{
	switch (Slot)
	{
	case ETDWeaponSlot::Frame:			return "Frame";
	case ETDWeaponSlot::Slide:				return "Slide";
	case ETDWeaponSlot::Hammer:		return "Hammer";
	case ETDWeaponSlot::Magazine:		return "Magazine";
	case ETDWeaponSlot::Barrel:			return "Barrel";
	case ETDWeaponSlot::Muzzle:			return "Muzzle";
	case ETDWeaponSlot::Sight:				return "Sight";
	case ETDWeaponSlot::Grip:				return "Grip";
	case ETDWeaponSlot::Trigger:			return "Trigger";
	case ETDWeaponSlot::Ejector:			return "Ejector";
	default:													return NAME_None;
	}
}