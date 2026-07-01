#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TDPlayerController.generated.h"

class UTDW_AmmoWidget;
class UTDReloadBarWidget;
class UTDHitMarkerWidget;

UCLASS()
class TOPDOWNSHOOTER_API ATDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetAmmoWidget(UTDW_AmmoWidget* InWidget);
	void SetReloadBarWidget(UTDReloadBarWidget* InWidget);
	void SetHitMarkerWidget(UTDHitMarkerWidget* InWidget);

	bool GetIsIndoor() const { return bIsIndoor; }

protected:
	ATDPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

	UPROPERTY()
	UTDW_AmmoWidget* AmmoWidget = nullptr;
	UTDReloadBarWidget* ReloadBarWidget = nullptr;

	UPROPERTY()
	UTDHitMarkerWidget* HitMarkerWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
	bool bIsIndoor = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	FVector2D MouseOffset = FVector2D(20.f, 0.f);
	FVector2D ReloadWidgetOffset = FVector2D(20.f, 0.f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	FVector2D HitMarkerWidgetOffset = FVector2D(0.f, 0.f);

	FVector CachedMouseWorldLocation;
};
