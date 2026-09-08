#include "Animation/TDANS_EnemyMeleeHitWindow.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/TDEnemyCharacter.h"

void UTDANS_EnemyMeleeHitWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp)
	{
		return;
	}

	if (ATDEnemyCharacter* Enemy = Cast<ATDEnemyCharacter>(MeshComp->GetOwner()))
	{
		Enemy->SetAttackHitboxEnabled(true);
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[EnemyAttack] Melee hit window opened: %s"), *Enemy->GetName());
#endif
	}
}

void UTDANS_EnemyMeleeHitWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}

	if (ATDEnemyCharacter* Enemy = Cast<ATDEnemyCharacter>(MeshComp->GetOwner()))
	{
		Enemy->SetAttackHitboxEnabled(false);
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[EnemyAttack] Melee hit window closed: %s"), *Enemy->GetName());
#endif
	}
}
