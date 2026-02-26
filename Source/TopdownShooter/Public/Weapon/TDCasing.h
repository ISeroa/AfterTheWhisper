#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDCasing.generated.h"

class UStaticMeshComponent;

UCLASS()
class TOPDOWNSHOOTER_API ATDCasing : public AActor
{
	GENERATED_BODY()

public:
	ATDCasing();
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Casing")
	UStaticMeshComponent* MeshComp;
};
