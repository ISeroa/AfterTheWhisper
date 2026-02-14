// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TDPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNSHOOTER_API ATDPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ATDPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

	FVector CachedMouseWorldLocation;
};
