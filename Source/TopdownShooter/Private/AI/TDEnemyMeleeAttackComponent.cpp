#include "AI/TDEnemyMeleeAttackComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"

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
	bHasHitCurrentAttack = false;

	OnMeleeAttackStarted.Broadcast(Target);

	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimerHandle,
		this,
		&UTDEnemyMeleeAttackComponent::ResetCooldown,
		Cooldown,
		false
	);
}

void UTDEnemyMeleeAttackComponent::TryApplyHit(AActor* HitActor)
{
	if (!HitActor)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	if (HitActor == Owner)
	{
		return;
	}

	AActor* Target = PendingTarget.Get();
	if (!Target)
	{
		return;
	}

	if (HitActor != Target)
	{
		return;
	}

	if (bHasHitCurrentAttack)
	{
		return;
	}

	bHasHitCurrentAttack = true;

	AController* InstigatorController = nullptr;
	if (APawn* OwnerPawn = Cast<APawn>(Owner))
	{
		InstigatorController = OwnerPawn->GetController();
	}

	UGameplayStatics::ApplyDamage(
		HitActor,
		AttackDamage,
		InstigatorController,
		Owner,
		DamageTypeClass
	);

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[EnemyAttack] Hit applied: Target=%s, Damage=%.1f"), *HitActor->GetName(), AttackDamage);
#endif
}

void UTDEnemyMeleeAttackComponent::FinishAttack()
{
	PendingTarget = nullptr;
	bHasHitCurrentAttack = false;
}

void UTDEnemyMeleeAttackComponent::ResetCooldown()
{
	bOnCooldown = false;
}

void UTDEnemyMeleeAttackComponent::StopAttack()
{
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
	PendingTarget = nullptr;
	bHasHitCurrentAttack = false;
	bOnCooldown = false;
}
