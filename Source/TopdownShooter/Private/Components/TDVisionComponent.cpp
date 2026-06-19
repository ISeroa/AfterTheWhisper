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

	const FVector OwnerLoc = Owner->GetActorLocation();

	// 2D 벡터 (Z 제거)
	const FVector2D OwnerLoc2D(OwnerLoc.X, OwnerLoc.Y);
	const FVector2D TargetLoc2D(WorldLocation.X, WorldLocation.Y);
	const FVector2D ToTarget2D = TargetLoc2D - OwnerLoc2D;
	const float DistSq2D = ToTarget2D.SizeSquared();

	// NearVision: 반경 안이면 방향 무관하게 통과
	if (DistSq2D <= FMath::Square(NearVisionRadius))
	{
		return true;
	}

	// ConeVision: 전방 콘 거리 체크
	if (DistSq2D > FMath::Square(ConeVisionDistance))
	{
		return false;
	}

	// 콘 각도 체크: Owner ForwardVector 기준 2D
	const FVector Forward3D = Owner->GetActorForwardVector();
	const FVector2D Forward2D(Forward3D.X, Forward3D.Y);
	if (Forward2D.IsNearlyZero())
	{
		return false;
	}

	const FVector2D ForwardNorm = Forward2D.GetSafeNormal();
	const FVector2D ToTargetNorm = ToTarget2D.GetSafeNormal();
	if (ToTargetNorm.IsNearlyZero())
	{
		return false;
	}

	const float DotProduct = FVector2D::DotProduct(ForwardNorm, ToTargetNorm);
	const float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(ConeHalfAngleDeg));

	return DotProduct >= CosHalfAngle;
}

bool UTDVisionComponent::IsActorVisible(const AActor* TargetActor) const
{
	const AActor* Owner = GetOwner();
	if (!Owner || !TargetActor) return false;

	const FVector TargetLoc = TargetActor->GetActorLocation();

	if (!IsLocationInVision(TargetLoc)) return false;

	if (!bUseLineOfSightCheck) return true;

	UWorld* World = Owner->GetWorld();
	if (!World) return false;

	const FVector OwnerLoc = Owner->GetActorLocation();

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	const bool bHit = World->LineTraceSingleByChannel(HitResult, OwnerLoc, TargetLoc, VisionTraceChannel, Params);
	const bool bVisible = !bHit || HitResult.GetActor() == TargetActor;

#if !UE_BUILD_SHIPPING
	if (bDebugVision)
	{
		if (bVisible)
		{
			DrawDebugLine(World, OwnerLoc, TargetLoc, FColor::Green, false, -1.f, 0, 2.f);
		}
		else
		{
			DrawDebugLine(World, OwnerLoc, HitResult.ImpactPoint, FColor::Red, false, -1.f, 0, 2.f);
			DrawDebugPoint(World, HitResult.ImpactPoint, 8.f, FColor::Red, false, -1.f);
		}
	}
#endif

	return bVisible;
}

void UTDVisionComponent::DrawDebugVision() const
{
#if !UE_BUILD_SHIPPING
	if (!bDebugVision) return;

	const AActor* Owner = GetOwner();
	if (!Owner) return;

	UWorld* World = Owner->GetWorld();
	if (!World) return;

	const FVector OwnerLoc = Owner->GetActorLocation();

	// 근거리 원형
	DrawDebugCircle(
		World,
		OwnerLoc,
		NearVisionRadius,
		32,
		FColor::Cyan,
		false,
		-1.f,
		0,
		2.f,
		FVector(1.f, 0.f, 0.f),
		FVector(0.f, 1.f, 0.f)
	);

	// 전방 콘
	const FVector Forward3D = Owner->GetActorForwardVector();
	const FVector2D Forward2D(Forward3D.X, Forward3D.Y);
	if (Forward2D.IsNearlyZero()) return;

	const FVector2D ForwardNorm = Forward2D.GetSafeNormal();
	const float HalfAngleRad = FMath::DegreesToRadians(ConeHalfAngleDeg);

	// 중심선 (Green)
	const FVector ConeEnd = OwnerLoc + FVector(ForwardNorm.X, ForwardNorm.Y, 0.f) * ConeVisionDistance;
	DrawDebugLine(World, OwnerLoc, ConeEnd, FColor::Green, false, -1.f, 0, 2.f);

	// 좌측 경계선 (Yellow)
	const float CosL = FMath::Cos(-HalfAngleRad);
	const float SinL = FMath::Sin(-HalfAngleRad);
	const FVector2D LeftDir(
		ForwardNorm.X * CosL - ForwardNorm.Y * SinL,
		ForwardNorm.X * SinL + ForwardNorm.Y * CosL
	);
	const FVector LeftEnd = OwnerLoc + FVector(LeftDir.X, LeftDir.Y, 0.f) * ConeVisionDistance;
	DrawDebugLine(World, OwnerLoc, LeftEnd, FColor::Yellow, false, -1.f, 0, 2.f);

	// 우측 경계선 (Yellow)
	const float CosR = FMath::Cos(HalfAngleRad);
	const float SinR = FMath::Sin(HalfAngleRad);
	const FVector2D RightDir(
		ForwardNorm.X * CosR - ForwardNorm.Y * SinR,
		ForwardNorm.X * SinR + ForwardNorm.Y * CosR
	);
	const FVector RightEnd = OwnerLoc + FVector(RightDir.X, RightDir.Y, 0.f) * ConeVisionDistance;
	DrawDebugLine(World, OwnerLoc, RightEnd, FColor::Yellow, false, -1.f, 0, 2.f);

	// 콘 끝부분 호 (Yellow): -HalfAngle ~ +HalfAngle 구간을 16 세그먼트로 근사
	const int32 ArcSegments = 16;
	const float AngleStep = (2.f * HalfAngleRad) / ArcSegments;
	FVector PrevArcPoint = LeftEnd;
	for (int32 i = 1; i <= ArcSegments; ++i)
	{
		const float Angle = -HalfAngleRad + AngleStep * i;
		const float CosA = FMath::Cos(Angle);
		const float SinA = FMath::Sin(Angle);
		const FVector2D ArcDir(
			ForwardNorm.X * CosA - ForwardNorm.Y * SinA,
			ForwardNorm.X * SinA + ForwardNorm.Y * CosA
		);
		const FVector ArcPoint = OwnerLoc + FVector(ArcDir.X, ArcDir.Y, 0.f) * ConeVisionDistance;
		DrawDebugLine(World, PrevArcPoint, ArcPoint, FColor::Yellow, false, -1.f, 0, 2.f);
		PrevArcPoint = ArcPoint;
	}
#endif
}
