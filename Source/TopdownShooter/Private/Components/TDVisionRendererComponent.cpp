// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/TDVisionRendererComponent.h"
#include "Components/TDVisionComponent.h"
#include "Character/TDEnemyCharacter.h"
#include "DrawDebugHelpers.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CanvasItem.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "RenderUtils.h"

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

	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATDEnemyCharacter::StaticClass(), FoundEnemies);
	for (AActor* Enemy : FoundEnemies)
	{
		VisionTraceIgnoredActors.Add(Enemy);
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
	for (const TWeakObjectPtr<AActor>& IgnoredActor : VisionTraceIgnoredActors)
	{
		if (IgnoredActor.IsValid())
		{
			Params.AddIgnoredActor(IgnoredActor.Get());
		}
	}

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

	DrawToRenderTarget();
}

void UTDVisionRendererComponent::DrawToRenderTarget()
{
	if (!VisionMaskRenderTarget || VisibilityPolygonPoints.Num() < 2) return;

	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// Viewport 크기 → RT 좌표 비율 계산
	int32 ViewportW = 0, ViewportH = 0;
	PC->GetViewportSize(ViewportW, ViewportH);
	if (ViewportW == 0 || ViewportH == 0) return;

	const float ScaleX = (float)VisionMaskRenderTarget->SizeX / (float)ViewportW;
	const float ScaleY = (float)VisionMaskRenderTarget->SizeY / (float)ViewportH;

	// 플레이어 위치 → RT 좌표
	FVector2D OwnerScreenPos;
	if (!PC->ProjectWorldLocationToScreen(Owner->GetActorLocation(), OwnerScreenPos, false)) return;
	const FVector2D CenterRT(OwnerScreenPos.X * ScaleX, OwnerScreenPos.Y * ScaleY);

	// 폴리곤 포인트 → RT 좌표
	TArray<FVector2D> RTPoints;
	RTPoints.Reserve(VisibilityPolygonPoints.Num());
	for (const FVector& WorldPt : VisibilityPolygonPoints)
	{
		FVector2D ScreenPt;
		PC->ProjectWorldLocationToScreen(WorldPt, ScreenPt, false);
		RTPoints.Add(FVector2D(ScreenPt.X * ScaleX, ScreenPt.Y * ScaleY));
	}

	// Canvas 열기
	UCanvas* Canvas = nullptr;
	FVector2D CanvasSize;
	FDrawToRenderTargetContext Context;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(World, VisionMaskRenderTarget, Canvas, CanvasSize, Context);

	if (!Canvas)
	{
		UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(World, Context);
		return;
	}

	// 검은 배경으로 초기화
	FCanvasTileItem BGItem(FVector2D::ZeroVector, CanvasSize, FLinearColor::Black);
	BGItem.BlendMode = SE_BLEND_Opaque;
	Canvas->DrawItem(BGItem);

	// 흰색 삼각형 Fan: 플레이어 위치를 중심으로 인접한 두 포인트마다 삼각형 하나
	const int32 Num = RTPoints.Num();
	for (int32 i = 0; i < Num; ++i)
	{
		FCanvasTriangleItem TriItem(CenterRT, RTPoints[i], RTPoints[(i + 1) % Num], GWhiteTexture);
		TriItem.BlendMode = SE_BLEND_Opaque;
		Canvas->DrawItem(TriItem);
	}

	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(World, Context);
}
