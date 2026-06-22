// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TDVisionRendererComponent.generated.h"

class UTDVisionComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TOPDOWNSHOOTER_API UTDVisionRendererComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTDVisionRendererComponent();

	const TArray<FVector>& GetVisibilityPolygonPoints() const { return VisibilityPolygonPoints; }

	/** 폴리곤 갱신 주기 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vision|Renderer")
	float UpdateInterval = 0.1f;

	/** 360도 샘플링 Ray 수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vision|Renderer")
	int32 RayCount = 128;

	/** true면 Visibility Polygon을 Debug Line으로 표시한다 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vision|Renderer|Debug")
	bool bDebugVisionPolygon = false;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void UpdatePolygon();

	UPROPERTY()
	UTDVisionComponent* CachedVisionComp = nullptr;

	TArray<FVector> VisibilityPolygonPoints;

	FTimerHandle PolygonTimerHandle;
};
