// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/TDGameMode.h"
#include "Core/TDPlayerController.h"
#include "Character/TDPlayerCharacter.h"

ATDGameMode::ATDGameMode()
{
	DefaultPawnClass = ATDPlayerCharacter::StaticClass();
	PlayerControllerClass = ATDPlayerController::StaticClass();
}

bool ATDGameMode::IsGameWon() const
{
	return bIsGameWon;
}

bool ATDGameMode::IsGameLost() const
{
	return bIsGameLost;
}

bool ATDGameMode::IsGameFinished() const
{
	return bIsGameWon || bIsGameLost;
}

void ATDGameMode::CompleteGameAsVictory()
{
	if (bIsGameWon || bIsGameLost) return;

	bIsGameWon = true;

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Warning, TEXT("[GameMode] Game completed as victory"));
#endif

	OnGameWon();
}

void ATDGameMode::CompleteGameAsDefeat()
{
	if (bIsGameWon || bIsGameLost) return;

	bIsGameLost = true;

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Warning, TEXT("[GameMode] Game completed as defeat"));
#endif

	OnGameLost();
}
