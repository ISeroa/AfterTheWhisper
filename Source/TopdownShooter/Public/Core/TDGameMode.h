// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TDGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNSHOOTER_API ATDGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATDGameMode();

	UFUNCTION(BlueprintPure, Category = "Game State")
	bool IsGameWon() const;

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void CompleteGameAsVictory();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Game State")
	void OnGameWon();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	bool bIsGameWon = false;
};
