#include "AI/TDEnemyMeleeAttackComponent.h"
#include "Kismet/GameplayStatics.h"

UTDEnemyMeleeAttackComponent::UTDEnemyMeleeAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTDEnemyMeleeAttackComponent::TryAttack(AActor* Target)
{
	if (bOnCooldown || !Target)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	const float RangeSq = AttackRange * AttackRange;
	if (FVector::DistSquared(Owner->GetActorLocation(), Target->GetActorLocation()) > RangeSq)
	{
		return;
	}

	bOnCooldown = true;
	PendingTarget = Target;

	GetWorld()->GetTimerManager().SetTimer(
		WindupTimerHandle,
		this,
		&UTDEnemyMeleeAttackComponent::ExecuteHit,
		WindupTime,
		false
	);
}

void UTDEnemyMeleeAttackComponent::ExecuteHit()
{
	AActor* Target = PendingTarget.Get();
	AActor* Owner  = GetOwner();

	if (Target && Owner)
	{
		// 윈드업 중 타깃이 범위를 벗어났으면 판정 취소
		const float RangeSq = AttackRange * AttackRange;
		if (FVector::DistSquared(Owner->GetActorLocation(), Target->GetActorLocation()) <= RangeSq)
		{
			AController* InstigatorController = nullptr;
			if (APawn* OwnerPawn = Cast<APawn>(Owner))
			{
				InstigatorController = OwnerPawn->GetController();
			}

			UGameplayStatics::ApplyDamage(
				Target,
				AttackDamage,
				InstigatorController,
				Owner,
				DamageTypeClass
			);
		}
	}

	PendingTarget = nullptr;

	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimerHandle,
		this,
		&UTDEnemyMeleeAttackComponent::ResetCooldown,
		Cooldown,
		false
	);
}

void UTDEnemyMeleeAttackComponent::ResetCooldown()
{
	bOnCooldown = false;
}
