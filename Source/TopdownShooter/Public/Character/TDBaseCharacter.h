// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TDBaseCharacter.generated.h"

class UTDHealthComponent;

UCLASS()
class TOPDOWNSHOOTER_API ATDBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATDBaseCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	UTDHealthComponent* HealthComponent = nullptr;

public:	
	UFUNCTION()
	void HandleDeath();
};
