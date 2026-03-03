#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TDEnemyAIController.generated.h"

UCLASS()
class TOPDOWNSHOOTER_API ATDEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float RepathInterval = 0.35f;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	FTimerHandle RepathTimerHandle;

	void UpdateMoveTarget();
};
