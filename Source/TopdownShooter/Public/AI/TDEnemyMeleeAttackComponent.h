#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TDEnemyMeleeAttackComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TOPDOWNSHOOTER_API UTDEnemyMeleeAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTDEnemyMeleeAttackComponent();

	// AIController가 매 RepathInterval마다 호출.
	// 쿨다운 중이거나 AttackRange 밖이면 무시.
	void TryAttack(AActor* Target);

	// 사망 시 진행 중인 윈드업/쿨다운 타이머 즉시 정리
	void StopAttack();

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackRange = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackDamage = 20.f;

	// 공격 판정까지의 선딜레이
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float WindupTime = 0.3f;

	// 판정 이후 다음 TryAttack 수락까지의 대기 시간
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float Cooldown = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<UDamageType> DamageTypeClass;

private:
	void ExecuteHit();
	void ResetCooldown();

	FTimerHandle WindupTimerHandle;
	FTimerHandle CooldownTimerHandle;

	bool bOnCooldown = false;

	// 윈드업 중 타깃 보관 (사망 시 자동 null)
	TWeakObjectPtr<AActor> PendingTarget;
};
