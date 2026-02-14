// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TDHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTDOnHealthChanged, float, NewHealth, float, Delat);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTDOnDeath);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TOPDOWNSHOOTER_API UTDHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTDHealthComponent();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Health")
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Health")
	float CurrentHealth = 100.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Health")
	bool bDead = false;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FTDOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FTDOnDeath OnDeath;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	void Die();
};
