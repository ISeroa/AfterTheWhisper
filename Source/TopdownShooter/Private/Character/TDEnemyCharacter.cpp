#include "Character/TDEnemyCharacter.h"
#include "AI/TDEnemyAIController.h"

ATDEnemyCharacter::ATDEnemyCharacter()
{
	AIControllerClass = ATDEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}
