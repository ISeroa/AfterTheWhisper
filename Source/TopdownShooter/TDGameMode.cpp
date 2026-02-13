// Fill out your copyright notice in the Description page of Project Settings.

#include "TDGameMode.h"
#include "TDPlayerCharacter.h"
#include "TDPlayerController.h"

ATDGameMode::ATDGameMode()
{
	DefaultPawnClass = ATDPlayerCharacter::StaticClass();
	PlayerControllerClass = ATDPlayerController::StaticClass();
}
