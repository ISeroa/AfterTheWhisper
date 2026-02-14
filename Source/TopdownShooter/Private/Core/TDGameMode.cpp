// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/TDGameMode.h"
#include "Core/TDPlayerController.h"
#include "Character/TDPlayerCharacter.h"

ATDGameMode::ATDGameMode()
{
	DefaultPawnClass = ATDPlayerCharacter::StaticClass();
	PlayerControllerClass = ATDPlayerController::StaticClass();
}
