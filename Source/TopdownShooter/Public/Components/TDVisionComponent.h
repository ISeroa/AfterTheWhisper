// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TDVisionComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TOPDOWNSHOOTER_API UTDVisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTDVisionComponent();

	/**
	 * 특정 월드 위치가 시야 안에 있는지 검사한다.
	 * NearVisionRadius 원형 OR 전방 콘 중 하나를 통과하면 true.
	 * 거리/방향 판정은 Z를 제거한 2D 기준.
	 */
	bool IsLocationInVision(const FVector& WorldLocation) const;

	/** bDebugVision이 켜져 있을 때 원형 및 콘 디버그 라인을 그린다. ATDPlayerCharacter::Tick에서 호출한다. */
	void DrawDebugVision() const;

	/** 방향과 무관한 근거리 원형 시야 반경 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vision")
	float NearVisionRadius = 600.f;

	/** 전방 콘 시야 최대 거리 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vision")
	float ConeVisionDistance = 1400.f;

	/** 전방 콘 시야 반각 (도, 단방향) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vision")
	float ConeHalfAngleDeg = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vision|Debug")
	bool bDebugVision = false;
};
