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

	UPROPERTY(EditDefaultsOnly, Category = "AI|Encircle")
	float EncircleRadius = 280.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Encircle")
	float Jitter = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Encircle", meta = (ClampMin = "1"))
	int32 NumSlots = 12;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Encircle")
	float OccupancyRadius = 160.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Encircle")
	float OccupancyPenalty = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Encircle")
	float SameSlotPenalty = 100000.f;

	// 현재 선택된 슬롯 인덱스 — 다른 컨트롤러가 읽어서 중복 회피에 사용
	int32 CurrentSlotIndex = INDEX_NONE;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	FTimerHandle RepathTimerHandle;
	float RadiusBias = 0.f;

	APawn*  GetPlayerPawn() const;
	FVector ComputeMoveGoal(APawn* Player);
	FVector ComputeEncircleGoal(APawn* Player);
	void    UpdateMoveTarget();
};
