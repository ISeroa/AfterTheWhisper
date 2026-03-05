#include "Character/TDEnemyCharacter.h"
#include "AI/TDEnemyAIController.h"
#include "AI/TDEnemyMeleeAttackComponent.h"

ATDEnemyCharacter::ATDEnemyCharacter()
{
	AIControllerClass = ATDEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	MeleeAttackComp = CreateDefaultSubobject<UTDEnemyMeleeAttackComponent>(TEXT("MeleeAttackComp"));
}
