#include "AI/TDEnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

#if !UE_BUILD_SHIPPING
#include "DrawDebugHelpers.h"
#endif

void ATDEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Log, TEXT("[TDEnemyAI] OnPossess: %s -> %s"),
		*GetName(), InPawn ? *InPawn->GetName() : TEXT("NULL"));

	CurrentSlotIndex = INDEX_NONE;
	RadiusBias = FMath::RandRange(-Jitter, Jitter);
	UE_LOG(LogTemp, Log, TEXT("[TDEnemyAI] RadiusBias=%.1f"), RadiusBias);

	// 적마다 첫 실행 타이밍을 다르게 해 동시 선택 충돌을 줄임
	const float InitialDelay = FMath::RandRange(0.f, RepathInterval);
	GetWorld()->GetTimerManager().SetTimer(
		RepathTimerHandle,
		this,
		&ATDEnemyAIController::UpdateMoveTarget,
		RepathInterval,
		true,
		InitialDelay
	);
	UE_LOG(LogTemp, Warning, TEXT("[EnemyAI] Timer set interval=%.2f, InitialDelay=%.2f"),
		RepathInterval, InitialDelay);
}

void ATDEnemyAIController::OnUnPossess()
{
	UE_LOG(LogTemp, Log, TEXT("[TDEnemyAI] OnUnPossess: %s"), *GetName());
	CurrentSlotIndex = INDEX_NONE;  // 해제 즉시 예약 취소
	GetWorld()->GetTimerManager().ClearTimer(RepathTimerHandle);
	Super::OnUnPossess();
}

APawn* ATDEnemyAIController::GetPlayerPawn() const
{
	return UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

FVector ATDEnemyAIController::ComputeEncircleGoal(APawn* Player)
{
	const FVector PlayerLoc = Player->GetActorLocation();

	APawn* EnemyPawn = GetPawn();
	const FVector SelfLoc = EnemyPawn ? EnemyPawn->GetActorLocation() : PlayerLoc;

	static const FVector WorldForward = FVector(1.f, 0.f, 0.f);
	static const FVector WorldRight   = FVector(0.f, 1.f, 0.f);

	const float AngleStep   = 2.f * PI / FMath::Max(NumSlots, 1);
	const float FinalRadius = EncircleRadius + RadiusBias;

	// 위치 기반 점유 체크용 폰 목록
	TArray<AActor*> AllPawns;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), AllPawns);

	float   BestCost     = TNumericLimits<float>::Max();
	int32   BestIndex    = 0;
	FVector BestSlotPos  = PlayerLoc + WorldForward * FinalRadius;

	for (int32 i = 0; i < NumSlots; ++i)
	{
		const float   Angle   = i * AngleStep;
		const FVector Dir     = WorldForward * FMath::Cos(Angle) + WorldRight * FMath::Sin(Angle);
		const FVector SlotPos = PlayerLoc + Dir * FinalRadius;

		// 거리 비용
		float Cost = FVector::Dist2D(SelfLoc, SlotPos);

		// 위치 기반 점유 패널티: 슬롯 반경 내 다른 적 수
		for (AActor* Actor : AllPawns)
		{
			APawn* Other = Cast<APawn>(Actor);
			if (!Other || Other == EnemyPawn || Other == Player)
			{
				continue;
			}
			if (FVector::Dist2D(Other->GetActorLocation(), SlotPos) < OccupancyRadius)
			{
				Cost += OccupancyPenalty;
			}
		}

		// 의도 기반 패널티: 이미 같은 슬롯을 목표로 삼은 다른 컨트롤러 수
		for (TActorIterator<ATDEnemyAIController> It(GetWorld()); It; ++It)
		{
			if (*It != this && (*It)->CurrentSlotIndex == i)
			{
				Cost += SameSlotPenalty;
			}
		}

		if (Cost < BestCost)
		{
			BestCost    = Cost;
			BestIndex   = i;
			BestSlotPos = SlotPos;
		}

#if !UE_BUILD_SHIPPING
		DrawDebugPoint(GetWorld(), SlotPos, 6.f, FColor::Silver, false, RepathInterval * 1.1f);
#endif
	}

	CurrentSlotIndex = BestIndex;
	return BestSlotPos;
}

FVector ATDEnemyAIController::ComputeMoveGoal(APawn* Player)
{
	switch (MovementTactic)
	{
	case ETDMovementTactic::DirectChase:
		return Player->GetActorLocation();

	case ETDMovementTactic::Encircle:
		return ComputeEncircleGoal(Player);

	default:
		return Player->GetActorLocation();
	}
}

void ATDEnemyAIController::UpdateMoveTarget()
{
	UE_LOG(LogTemp, Warning, TEXT("[EnemyAI] UpdateMoveTarget"));

	APawn* Player = GetPlayerPawn();
	if (!Player)
	{
		return;
	}

	const FVector Goal = ComputeMoveGoal(Player);
	MoveToLocation(Goal, AcceptanceRadius, /*bStopOnOverlap=*/true);

#if !UE_BUILD_SHIPPING
	DrawDebugSphere(GetWorld(), Goal, 24.f, 8, FColor::Green, false, RepathInterval * 1.1f);
#endif
}
