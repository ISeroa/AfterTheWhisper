#include "Animation/AnimNotify_Fire.h"

void UAnimNotify_Fire::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// Visual-only notify. Actual firing (line trace, damage, ammo) is handled by ATDWeaponBase.
}
