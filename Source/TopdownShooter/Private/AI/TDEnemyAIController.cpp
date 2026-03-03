#include "AI/TDEnemyAIController.h"
#include "Kismet/GameplayStatics.h"

void ATDEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Log, TEXT("[TDEnemyAI] OnPossess: %s -> %s"),
		*GetName(), InPawn ? *InPawn->GetName() : TEXT("NULL"));

	GetWorld()->GetTimerManager().SetTimer(
		RepathTimerHandle,
		this,
		&ATDEnemyAIController::UpdateMoveTarget,
		RepathInterval,
		true
	);
	UE_LOG(LogTemp, Warning, TEXT("[EnemyAI] Timer set interval=%.2f"), RepathInterval);
}

void ATDEnemyAIController::OnUnPossess()
{
	UE_LOG(LogTemp, Log, TEXT("[TDEnemyAI] OnUnPossess: %s"), *GetName());
	GetWorld()->GetTimerManager().ClearTimer(RepathTimerHandle);
	Super::OnUnPossess();
}

APawn* ATDEnemyAIController::GetPlayerPawn() const
{
	return UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

FVector ATDEnemyAIController::ComputeMoveGoal(APawn* Player) const
{
	switch (MovementTactic)
	{
	case ETDMovementTactic::DirectChase:
		return Player->GetActorLocation();

	case ETDMovementTactic::Encircle:
		// placeholder — Encircle 계산은 미구현
		return Player->GetActorLocation();

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
}
