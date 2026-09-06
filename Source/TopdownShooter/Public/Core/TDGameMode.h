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

	UFUNCTION(BlueprintPure, Category = "Game State")
	bool IsGameLost() const;

	UFUNCTION(BlueprintPure, Category = "Game State")
	bool IsGameFinished() const;

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void CompleteGameAsVictory();

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void CompleteGameAsDefeat();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Game State")
	void OnGameWon();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game State")
	void OnGameLost();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	bool bIsGameWon = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	bool bIsGameLost = false;
};
