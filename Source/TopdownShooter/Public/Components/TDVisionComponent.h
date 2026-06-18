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

	/** 특정 월드 위치가 시야 반경 안에 있는지 검사한다. Owner 위치 기준 2D 거리(DistSquared2D) 사용. */
	bool IsLocationInVision(const FVector& WorldLocation) const;

	/** bDebugVision이 켜져 있을 때 원형 디버그 라인을 그린다. ATDPlayerCharacter::Tick에서 호출한다. */
	void DrawDebugVision() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vision")
	float VisionRadius = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vision|Debug")
	bool bDebugVision = false;
};
