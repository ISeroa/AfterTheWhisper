#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TDPlayerController.generated.h"

class UTDW_AmmoWidget;

UCLASS()
class TOPDOWNSHOOTER_API ATDPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void SetAmmoWidget(UTDW_AmmoWidget* InWidget);

protected:
	ATDPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

	UPROPERTY()
	UTDW_AmmoWidget* AmmoWidget = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	FVector2D MouseOffset = FVector2D(20.f, 0.f);

	FVector CachedMouseWorldLocation;
};
