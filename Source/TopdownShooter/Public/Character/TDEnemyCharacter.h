#pragma once

#include "CoreMinimal.h"
#include "Character/TDBaseCharacter.h"
#include "TDEnemyCharacter.generated.h"

class UTDEnemyMeleeAttackComponent;

UCLASS()
class TOPDOWNSHOOTER_API ATDEnemyCharacter : public ATDBaseCharacter
{
	GENERATED_BODY()

public:
	ATDEnemyCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTDEnemyMeleeAttackComponent* MeleeAttackComp = nullptr;

	// 래그돌 임펄스 세기 (BP에서 튜닝)
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float RagdollImpulseStrength = 600.f;

	// 래그돌 상태로 머무는 시간 후 Destroy
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float RagdollLifeTime = 5.f;

protected:
	// 마지막 피격 발사 방향 (TakeDamage에서 갱신)
	FVector LastHitDirection = FVector::ZeroVector;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void HandleDeath() override;
};
