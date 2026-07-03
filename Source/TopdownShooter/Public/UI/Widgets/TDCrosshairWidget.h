#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDCrosshairWidget.generated.h"

UCLASS()
class TOPDOWNSHOOTER_API UTDCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void BP_UpdateCrosshair(float SpreadDeg, bool bIsAiming);
};
