// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/TDVisionComponent.h"
#include "DrawDebugHelpers.h"

UTDVisionComponent::UTDVisionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UTDVisionComponent::IsLocationInVision(const FVector& WorldLocation) const
{
	const AActor* Owner = GetOwner();
	if (!Owner) return false;

	const float DistSq2D = FVector::DistSquared2D(Owner->GetActorLocation(), WorldLocation);
	return DistSq2D <= FMath::Square(VisionRadius);
}

void UTDVisionComponent::DrawDebugVision() const
{
#if !UE_BUILD_SHIPPING
	if (!bDebugVision) return;

	const AActor* Owner = GetOwner();
	if (!Owner) return;

	DrawDebugCircle(
		Owner->GetWorld(),
		Owner->GetActorLocation(),
		VisionRadius,
		32,
		FColor::Cyan,
		false,
		-1.f,
		0,
		2.f,
		FVector(1.f, 0.f, 0.f),
		FVector(0.f, 1.f, 0.f)
	);
#endif
}
