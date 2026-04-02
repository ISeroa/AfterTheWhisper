#include "Character/TDEnemyCharacter.h"
#include "AI/TDEnemyAIController.h"
#include "AI/TDEnemyMeleeAttackComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/EngineTypes.h"

ATDEnemyCharacter::ATDEnemyCharacter()
{
	AIControllerClass = ATDEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	MeleeAttackComp = CreateDefaultSubobject<UTDEnemyMeleeAttackComponent>(TEXT("MeleeAttackComp"));
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

	// Ragdoll 전환
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
}
