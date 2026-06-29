#include "Character/TDEnemyCharacter.h"
#include "AI/TDEnemyAIController.h"
#include "AI/TDEnemyMeleeAttackComponent.h"
#include "Components/TDHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/EngineTypes.h"

ATDEnemyCharacter::ATDEnemyCharacter()
{
	AIControllerClass = ATDEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	MeleeAttackComp = CreateDefaultSubobject<UTDEnemyMeleeAttackComponent>(TEXT("MeleeAttackComp"));
}

void ATDEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		BaseWalkSpeed = MoveComp->MaxWalkSpeed;
	}
}

void ATDEnemyCharacter::ApplyHitReaction(ETDStoppingPowerTier Tier)
{
	if (HealthComponent && HealthComponent->bDead) return;
	if (Tier == ETDStoppingPowerTier::None) return;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp) return;

	GetWorldTimerManager().ClearTimer(TimerHandle_SlowRestore);
	GetWorldTimerManager().ClearTimer(TimerHandle_StunEnd);

	switch (Tier)
	{
	case ETDStoppingPowerTier::Light:
		MoveComp->MaxWalkSpeed = BaseWalkSpeed * 0.75f;
		GetWorldTimerManager().SetTimer(TimerHandle_SlowRestore, this,
			&ATDEnemyCharacter::RestoreWalkSpeed, 0.15f, false);
		break;

	case ETDStoppingPowerTier::Medium:
		MoveComp->MaxWalkSpeed = BaseWalkSpeed * 0.5f;
		GetWorldTimerManager().SetTimer(TimerHandle_SlowRestore, this,
			&ATDEnemyCharacter::RestoreWalkSpeed, 0.25f, false);
		break;

	case ETDStoppingPowerTier::Heavy:
		MoveComp->StopMovementImmediately();
		MoveComp->MaxWalkSpeed = 0.f;
		GetWorldTimerManager().SetTimer(TimerHandle_StunEnd, this,
			&ATDEnemyCharacter::EndStunBeginSlow, 0.12f, false);
		break;

	default:
		break;
	}
}

void ATDEnemyCharacter::RestoreWalkSpeed()
{
	if (HealthComponent && HealthComponent->bDead) return;
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = BaseWalkSpeed;
	}
}

void ATDEnemyCharacter::EndStunBeginSlow()
{
	if (HealthComponent && HealthComponent->bDead) return;
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = BaseWalkSpeed * 0.5f;
	}
	GetWorldTimerManager().SetTimer(TimerHandle_SlowRestore, this,
		&ATDEnemyCharacter::RestoreWalkSpeed, 0.2f, false);
}

float ATDEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	// FPointDamageEvent에서 발사 방향 직접 추출 (무기가 ApplyPointDamage 사용)
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& PointDmg = static_cast<const FPointDamageEvent&>(DamageEvent);
		LastHitDirection = PointDmg.ShotDirection;
	}
	else if (DamageCauser && DamageCauser != this)
	{
		FVector Dir = GetActorLocation() - DamageCauser->GetActorLocation();
		Dir.Z = 0.f;
		LastHitDirection = Dir.GetSafeNormal();
	}

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ATDEnemyCharacter::HandleDeath()
{
	// AI 중단 — OnUnPossess에서 RepathTimerHandle 자동 해제됨
	if (AController* C = GetController())
	{
		C->UnPossess();
	}

	// 근접 공격 타이머 중단
	if (MeleeAttackComp)
	{
		MeleeAttackComp->StopAttack();
	}

	// 이동 중단
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}

	// 캡슐 충돌 비활성화
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	switch (DeathMode)
	{
	case ETDEnemyDeathMode::Ragdoll:
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MeshComp->SetAllBodiesSimulatePhysics(true);
			MeshComp->SetSimulatePhysics(true);
			MeshComp->WakeAllRigidBodies();
			MeshComp->bBlendPhysics = true;

			if (!LastHitDirection.IsNearlyZero())
			{
				MeshComp->AddImpulseToAllBodiesBelow(
					LastHitDirection * RagdollImpulseStrength,
					NAME_None,
					/*bVelChange=*/false,
					/*bIncludeSelf=*/true
				);
			}
		}
		SetLifeSpan(RagdollLifeTime);
		break;

	case ETDEnemyDeathMode::Animation:
		// TODO: 사망 애니메이션 재생
		SetActorHiddenInGame(true);
		SetLifeSpan(2.f);
		break;

	case ETDEnemyDeathMode::ImmediateDestroy:
		Destroy();
		break;
	}
}
