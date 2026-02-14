// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TDHealthComponent.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UTDHealthComponent::UTDHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UTDHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHealth = MaxHealth;

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UTDHealthComponent::HandleTakeAnyDamage);
	}
}

void UTDHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Warning, TEXT("[Health] TakeAnyDamage: %s dmg=%.1f causer=%s"),
		*GetOwner()->GetName(),
		Damage,
		DamageCauser ? *DamageCauser->GetName() : TEXT("None"));
#endif

	if (bDead || Damage <= 0.f) return;

	const float OldHealth = CurrentHealth; 
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	OnHealthChanged.Broadcast(CurrentHealth, CurrentHealth - OldHealth);

	if (CurrentHealth <= 0.f)
	{
		Die();
	}
}

void UTDHealthComponent::Die()
{
	if (bDead) return;
	bDead = true;

	OnDeath.Broadcast();
}


