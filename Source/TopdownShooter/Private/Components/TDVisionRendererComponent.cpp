// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/TDVisionRendererComponent.h"
#include "Components/TDVisionComponent.h"
#include "DrawDebugHelpers.h"

UTDVisionRendererComponent::UTDVisionRendererComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTDVisionRendererComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		CachedVisionComp = Owner->FindComponentByClass<UTDVisionComponent>();
	}

	if (!CachedVisionComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDVisionRendererComponent: UTDVisionComponent not found on Owner '%s'."),
			*GetNameSafe(GetOwner()));
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		PolygonTimerHandle,
		this,
		&UTDVisionRendererComponent::UpdatePolygon,
		UpdateInterval,
		true
	);
}

void UTDVisionRendererComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(PolygonTimerHandle);
}

void UTDVisionRendererComponent::UpdatePolygon()
{
	if (!CachedVisionComp) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	UWorld* World = Owner->GetWorld();
	if (!World) return;

	const FVector OwnerLoc = Owner->GetActorLocation();
	const FVector Forward3D = Owner->GetActorForwardVector();
	const FVector2D Forward2D = FVector2D(Forward3D.X, Forward3D.Y).GetSafeNormal();
	const float ForwardAngleRad = FMath::Atan2(Forward2D.Y, Forward2D.X);
	const float ConeHalfRad = FMath::DegreesToRadians(CachedVisionComp->ConeHalfAngleDeg);

	// 균등 샘플 + 콘 좌우 경계 각도
	TArray<float> SampleAngles;
	SampleAngles.Reserve(RayCount + 2);

	for (int32 i = 0; i < RayCount; ++i)
	{
		SampleAngles.Add((2.f * PI * i) / static_cast<float>(RayCount));
	}

	auto NormalizeAngle = [](float A) -> float
	{
		while (A < 0.f)       A += 2.f * PI;
		while (A >= 2.f * PI) A -= 2.f * PI;
		return A;
	};

	SampleAngles.Add(NormalizeAngle(ForwardAngleRad - ConeHalfRad));
	SampleAngles.Add(NormalizeAngle(ForwardAngleRad + ConeHalfRad));

	SampleAngles.Sort();

	VisibilityPolygonPoints.Reset();
	VisibilityPolygonPoints.Reserve(SampleAngles.Num());

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	const float NearRadius   = CachedVisionComp->NearVisionRadius;
	const float ConeDist     = CachedVisionComp->ConeVisionDistance;
	const float ConeMaxDist  = FMath::Max(NearRadius, ConeDist);
	const float CosHalf      = FMath::Cos(ConeHalfRad);

	for (const float Angle : SampleAngles)
	{
		const FVector2D Dir2D(FMath::Cos(Angle), FMath::Sin(Angle));
		const FVector   Dir3D(Dir2D.X, Dir2D.Y, 0.f);

		const bool  bInCone = !Forward2D.IsNearlyZero() && (FVector2D::DotProduct(Forward2D, Dir2D) >= CosHalf);
		const float MaxDist = bInCone ? ConeMaxDist : NearRadius;

		const FVector TraceEnd = OwnerLoc + Dir3D * MaxDist;
		FVector HitPoint = TraceEnd;

		if (CachedVisionComp->bUseLineOfSightCheck)
		{
			FHitResult Hit;
			if (World->LineTraceSingleByChannel(Hit, OwnerLoc, TraceEnd, CachedVisionComp->VisionTraceChannel, Params))
			{
				HitPoint = Hit.ImpactPoint;
			}
		}

		VisibilityPolygonPoints.Add(HitPoint);
	}

#if !UE_BUILD_SHIPPING
	if (bDebugVisionPolygon && VisibilityPolygonPoints.Num() > 1)
	{
		const int32 Num = VisibilityPolygonPoints.Num();
		for (int32 i = 0; i < Num; ++i)
		{
			const FVector& A = VisibilityPolygonPoints[i];
			const FVector& B = VisibilityPolygonPoints[(i + 1) % Num];
			DrawDebugLine(World, A, B, FColor::Orange, false, UpdateInterval * 1.5f, 0, 1.f);
		}
	}
#endif
}
