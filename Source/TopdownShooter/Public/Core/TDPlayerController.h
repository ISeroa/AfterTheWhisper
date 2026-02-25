#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TDPlayerController.generated.h"

class UTDW_AmmoWidget;
class UTDReloadBarWidget;

UCLASS()
class TOPDOWNSHOOTER_API ATDPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void SetAmmoWidget(UTDW_AmmoWidget* InWidget);
	void SetReloadBarWidget(UTDReloadBarWidget* InWidget);

	bool GetIsIndoor() const { return bIsIndoor; }
	
protected:
	ATDPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

	UPROPERTY()
	UTDW_AmmoWidget* AmmoWidget = nullptr;
	UTDReloadBarWidget* ReloadBarWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World")
	bool bIsIndoor = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	FVector2D MouseOffset = FVector2D(20.f, 0.f);
	FVector2D ReloadWidgetOffset = FVector2D(20.f, 0.f);

	FVector CachedMouseWorldLocation;
};
