#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "TDANS_EnemyMeleeHitWindow.generated.h"

class UAnimSequenceBase;
class USkeletalMeshComponent;

// 공격 Montage에서 실제 판정이 유효한 구간 동안 AttackHitbox를 켜고 끔.
// 공격별 런타임 상태는 저장하지 않는다 (ATDEnemyCharacter가 담당).
UCLASS()
class TOPDOWNSHOOTER_API UTDANS_EnemyMeleeHitWindow : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
