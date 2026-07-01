#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDHitMarkerWidget.generated.h"

UCLASS()
class TOPDOWNSHOOTER_API UTDHitMarkerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void BP_ShowHitMarker();
};
