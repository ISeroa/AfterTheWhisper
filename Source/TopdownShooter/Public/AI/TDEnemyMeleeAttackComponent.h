#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TDEnemyMeleeAttackComponent.generated.h"

class AActor;

// 실제 공격이 받아들여져 윈드업이 시작되는 시점에 Broadcast. C++ 전용(BlueprintAssignable 아님).
DECLARE_MULTICAST_DELEGATE_OneParam(FTDOnMeleeAttackStarted, AActor*);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TOPDOWNSHOOTER_API UTDEnemyMeleeAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTDEnemyMeleeAttackComponent();

	// AIController가 매 RepathInterval마다 호출.
	// 쿨다운 중이거나 AttackRange 밖이면 무시.
	void TryAttack(AActor* Target);

	// AttackHitbox가 Overlap한 Actor를 전달받아 PendingTarget과 일치하는지 검증 후 데미지 적용 (공격당 1회)
	void TryApplyHit(AActor* HitActor);

	// Montage 종료(정상/중단) 시 EnemyCharacter가 호출. PendingTarget/중복 방지 상태 정리 (Cooldown은 유지)
	void FinishAttack();

	// 사망 시 진행 중인 쿨다운 타이머 즉시 정리
	void StopAttack();

	// EnemyCharacter가 AddUObject로 구독해 공격 시작 시점(Montage 재생 등)을 처리
	FTDOnMeleeAttackStarted OnMeleeAttackStarted;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackRange = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackDamage = 20.f;

	// 판정 이후 다음 TryAttack 수락까지의 대기 시간
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float Cooldown = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<UDamageType> DamageTypeClass;

private:
	void ResetCooldown();

	FTimerHandle CooldownTimerHandle;

	bool bOnCooldown = false;

	// 공격 진행 중 타깃 보관 (사망 시 자동 null)
	TWeakObjectPtr<AActor> PendingTarget;

	// 현재 공격에서 이미 데미지를 적용했는지 여부 (중복 Overlap 방지)
	bool bHasHitCurrentAttack = false;
};
