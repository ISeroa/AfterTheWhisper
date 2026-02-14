// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TDBaseCharacter.h"
#include "Components/TDHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

ATDBaseCharacter::ATDBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject< UTDHealthComponent>(TEXT("HealthComponent"));
}

void ATDBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ATDBaseCharacter::HandleDeath);
	}
}

void ATDBaseCharacter::HandleDeath()
{
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	SetActorHiddenInGame(true);
	SetLifeSpan(2.f);
}

