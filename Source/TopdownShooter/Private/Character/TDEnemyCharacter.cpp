#include "Character/TDEnemyCharacter.h"
#include "AI/TDEnemyAIController.h"
#include "AI/TDEnemyMeleeAttackComponent.h"
#include "Components/TDHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/EngineTypes.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AIController.h"

ATDEnemyCharacter::ATDEnemyCharacter()
{
	AIControllerClass = ATDEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	MeleeAttackComp = CreateDefaultSubobject<UTDEnemyMeleeAttackComponent>(TEXT("MeleeAttackComp"));

	AttackHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("AttackHitbox"));
	AttackHitbox->SetupAttachment(GetMesh());
	AttackHitbox->SetSphereRadius(45.0f);
	AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackHitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackHitbox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	AttackHitbox->SetGenerateOverlapEvents(true);
}

void ATDEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		BaseWalkSpeed = MoveComp->MaxWalkSpeed;
	}

	if (MeleeAttackComp)
	{
		MeleeAttackComp->OnMeleeAttackStarted.AddUObject(this, &ATDEnemyCharacter::HandleMeleeAttackStarted);
	}

	if (!AttackHitbox || !GetMesh())
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("[EnemyAttack] Failed to attach AttackHitbox: component or mesh is null"));
#endif
	}
	else if (AttackHitboxSocketName == NAME_None)
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("[EnemyAttack] AttackHitboxSocketName is not assigned"));
#endif
	}
	else if (!GetMesh()->DoesSocketExist(AttackHitboxSocketName))
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("[EnemyAttack] AttackHitbox socket not found: %s"), *AttackHitboxSocketName.ToString());
#endif
	}
	else
	{
		AttackHitbox->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			AttackHitboxSocketName
		);
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[EnemyAttack] AttackHitbox attached to socket: %s"), *AttackHitboxSocketName.ToString());
#endif
	}

	if (AttackHitbox)
	{
		AttackHitbox->OnComponentBeginOverlap.AddDynamic(this, &ATDEnemyCharacter::OnAttackHitboxBeginOverlap);
	}
}

void ATDEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MeleeAttackComp)
	{
		MeleeAttackComp->OnMeleeAttackStarted.RemoveAll(this);
	}

	if (AttackHitbox)
	{
		AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	bIsPerformingAttack = false;

	Super::EndPlay(EndPlayReason);
}

void ATDEnemyCharacter::SetAttackHitboxEnabled(bool bEnabled)
{
	if (!AttackHitbox)
	{
		return;
	}

	if (bEnabled)
	{
		AttackHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[EnemyAttack] AttackHitbox enabled"));
#endif
	}
	else
	{
		AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[EnemyAttack] AttackHitbox disabled"));
#endif
	}
}

void ATDEnemyCharacter::OnAttackHitboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}

	if (OtherActor == GetOwner() || OtherActor == this)
	{
		return;
	}

	if (MeleeAttackComp)
	{
		MeleeAttackComp->TryApplyHit(OtherActor);
	}
}

void ATDEnemyCharacter::HandleMeleeAttackStarted(AActor* Target)
{
	if (!IsValid(Target))
	{
		return;
	}

	if (!AttackMontage)
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("[EnemyAttack] AttackMontage is not assigned"));
#endif
		return;
	}

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		return;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("[EnemyAttack] AnimInstance is null"));
#endif
		return;
	}

	if (AnimInstance->Montage_IsPlaying(AttackMontage))
	{
		return;
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	FVector Direction = Target->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;

	if (!Direction.IsNearlyZero())
	{
		FRotator FacingRotation = Direction.Rotation();
		FacingRotation.Pitch = 0.0f;
		FacingRotation.Roll = 0.0f;
		SetActorRotation(FacingRotation);
	}

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[EnemyAttack] Facing target: Target=%s, Yaw=%.2f"),
		*Target->GetName(), GetActorRotation().Yaw);
#endif

	bIsPerformingAttack = true;

	const float MontageLength = AnimInstance->Montage_Play(AttackMontage);
	if (MontageLength <= 0.0f)
	{
		bIsPerformingAttack = false;

		if (MeleeAttackComp)
		{
			MeleeAttackComp->FinishAttack();
		}

#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("[EnemyAttack] Failed to play attack montage"));
#endif
		return;
	}

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[EnemyAttack] Attack montage started: %s"), *AttackMontage->GetName());
#endif

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ATDEnemyCharacter::HandleAttackMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
}

void ATDEnemyCharacter::HandleAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != AttackMontage)
	{
		return;
	}

	bIsPerformingAttack = false;

	if (AttackHitbox)
	{
		AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (MeleeAttackComp)
	{
		MeleeAttackComp->FinishAttack();
	}

#if !UE_BUILD_SHIPPING
	if (bInterrupted)
	{
		UE_LOG(LogTemp, Log, TEXT("[EnemyAttack] Attack montage interrupted"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[EnemyAttack] Attack montage ended"));
	}
#endif
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

	// 사망 시 AttackHitbox가 켜져 있을 가능성 방지
	if (AttackHitbox)
	{
		AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	bIsPerformingAttack = false;

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
