#include "AI/TDEnemyAIController.h"

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
}

void ATDEnemyAIController::OnUnPossess()
{
	UE_LOG(LogTemp, Log, TEXT("[TDEnemyAI] OnUnPossess: %s"), *GetName());
	GetWorld()->GetTimerManager().ClearTimer(RepathTimerHandle);
	Super::OnUnPossess();
}

void ATDEnemyAIController::UpdateMoveTarget()
{
	UE_LOG(LogTemp, Log, TEXT("[TDEnemyAI] UpdateMoveTarget: tick"));
}
