#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDCasing.generated.h"

class UStaticMeshComponent;
class USoundBase;

UCLASS()
class TOPDOWNSHOOTER_API ATDCasing : public AActor
{
	GENERATED_BODY()

public:
	ATDCasing();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Casing")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Casing|Audio")
	USoundBase* ImpactSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Casing|Audio")
	float MinImpactSpeed = 100.f;

private:
	bool bPlayedImpact = false;
};
