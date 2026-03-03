#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TDEnemyAIController.generated.h"

UENUM(BlueprintType)
enum class ETDMovementTactic : uint8
{
	DirectChase,
	Encircle,
};

UCLASS()
class TOPDOWNSHOOTER_API ATDEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float RepathInterval = 0.35f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	ETDMovementTactic MovementTactic = ETDMovementTactic::DirectChase;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float AcceptanceRadius = 60.f;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	FTimerHandle RepathTimerHandle;

	APawn* GetPlayerPawn() const;
	FVector ComputeMoveGoal(APawn* Player) const;
	void UpdateMoveTarget();
};
