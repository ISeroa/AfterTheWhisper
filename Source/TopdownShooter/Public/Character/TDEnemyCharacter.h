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
};
