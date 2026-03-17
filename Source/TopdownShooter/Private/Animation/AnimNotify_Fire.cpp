#include "Animation/AnimNotify_Fire.h"
#include "Character/TDPlayerCharacter.h"

void UAnimNotify_Fire::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	ATDPlayerCharacter* Player = Cast<ATDPlayerCharacter>(MeshComp->GetOwner());
	if (Player)
	{
		Player->OnFireNotify();
	}
}
