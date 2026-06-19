// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/TDActorVisibilityComponent.h"
#include "Components/TDVisionComponent.h"
#include "Character/TDEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"

UTDActorVisibilityComponent::UTDActorVisibilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTDActorVisibilityComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		VisionComponent = Owner->FindComponentByClass<UTDVisionComponent>();
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATDEnemyCharacter::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (ATDEnemyCharacter* Enemy = Cast<ATDEnemyCharacter>(Actor))
		{
			TrackedEnemies.Add(Enemy);
			LastVisibilityState.Add(true); // 레벨 배치 Enemy는 기본적으로 visible
		}
	}

	GetWorld()->GetTimerManager().SetTimer(
		VisibilityTimerHandle,
		this,
		&UTDActorVisibilityComponent::UpdateVisibility,
		VisibilityUpdateInterval,
		true
	);

	// 첫 Timer를 기다리지 않고 초기 상태 즉시 적용
	UpdateVisibility();
}

void UTDActorVisibilityComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(VisibilityTimerHandle);
}

void UTDActorVisibilityComponent::UpdateVisibility()
{
	if (!VisionComponent) return;

	// 역순 순회: RemoveAtSwap으로 병렬 배열 동기화하며 유효하지 않은 항목 제거
	for (int32 i = TrackedEnemies.Num() - 1; i >= 0; --i)
	{
		if (!TrackedEnemies[i].IsValid())
		{
			TrackedEnemies.RemoveAtSwap(i);
			LastVisibilityState.RemoveAtSwap(i);
			continue;
		}

		ATDEnemyCharacter* Enemy = TrackedEnemies[i].Get();
		const bool bVisible = VisionComponent->IsActorVisible(Enemy);
		if (bVisible != LastVisibilityState[i])
		{
			Enemy->SetActorHiddenInGame(!bVisible);
			LastVisibilityState[i] = bVisible;
		}
	}
}
