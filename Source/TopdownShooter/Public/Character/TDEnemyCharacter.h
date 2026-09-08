#pragma once

#include "CoreMinimal.h"
#include "Character/TDBaseCharacter.h"
#include "Weapon/Types/TDWeaponTypes.h"
#include "TDEnemyCharacter.generated.h"

class UTDEnemyMeleeAttackComponent;
class UAnimMontage;
class USphereComponent;
class UPrimitiveComponent;

UENUM(BlueprintType)
enum class ETDEnemyDeathMode : uint8
{
	Ragdoll,          // 물리 기반 래그돌 (현재 구현)
	Animation,        // 사망 애니메이션 재생 (미구현)
	ImmediateDestroy, // 즉시 제거 (미구현)
};

UCLASS()
class TOPDOWNSHOOTER_API ATDEnemyCharacter : public ATDBaseCharacter
{
	GENERATED_BODY()

public:
	ATDEnemyCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTDEnemyMeleeAttackComponent* MeleeAttackComp = nullptr;

	// 이 적의 사망 처리 방식 (BP에서 적 타입별로 설정)
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	ETDEnemyDeathMode DeathMode = ETDEnemyDeathMode::Ragdoll;

	// 래그돌 임펄스 세기 (BP에서 튜닝)
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float RagdollImpulseStrength = 600.f;

	// 래그돌 상태로 머무는 시간 후 Destroy
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float RagdollLifeTime = 5.f;

	void ApplyHitReaction(ETDStoppingPowerTier Tier);

	// 근접 공격 시작 시 재생할 Montage (Blueprint에서 지정)
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* AttackMontage = nullptr;

	// 근접 공격 판정용 Sphere. 기본은 NoCollision — SetAttackHitboxEnabled()로 켜고 끔
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AttackHitbox = nullptr;

	// AttackHitbox를 부착할 Mesh 소켓 이름 (적 BP마다 지정, 예: hand_r). 기본은 미지정
	UPROPERTY(EditDefaultsOnly, Category = "Combat|AttackHitbox")
	FName AttackHitboxSocketName = NAME_None;

	// 외부(BP/AnimNotify 등)에서 Hitbox Collision을 켜고 끔
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetAttackHitboxEnabled(bool bEnabled);

	// AttackMontage 재생 중인지 여부. AI가 이동 명령을 보내기 전에 검사
	bool IsPerformingAttack() const { return bIsPerformingAttack; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 마지막 피격 발사 방향 (TakeDamage에서 갱신)
	FVector LastHitDirection = FVector::ZeroVector;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	virtual void HandleDeath() override;

private:
	float BaseWalkSpeed = 600.f;

	FTimerHandle TimerHandle_SlowRestore;
	FTimerHandle TimerHandle_StunEnd;

	void RestoreWalkSpeed();
	void EndStunBeginSlow();

	void HandleMeleeAttackStarted(AActor* Target);
	void HandleAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// AttackMontage 재생 중이면 true. AI 이동 갱신이 이 상태를 검사해 MoveToLocation/TryAttack을 막음
	bool bIsPerformingAttack = false;

	UFUNCTION()
	void OnAttackHitboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
