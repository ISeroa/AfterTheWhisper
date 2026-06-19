// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TDActorVisibilityComponent.generated.h"

class ATDEnemyCharacter;
class UTDVisionComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TOPDOWNSHOOTER_API UTDActorVisibilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTDActorVisibilityComponent();

	/** 가시성 갱신 주기 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visibility")
	float VisibilityUpdateInterval = 0.15f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void UpdateVisibility();

	UPROPERTY()
	UTDVisionComponent* VisionComponent = nullptr;

	// 병렬 배열: 같은 인덱스가 같은 Enemy와 그 이전 가시 상태를 가리킴
	TArray<TWeakObjectPtr<ATDEnemyCharacter>> TrackedEnemies;
	TArray<bool> LastVisibilityState;

	FTimerHandle VisibilityTimerHandle;
};
