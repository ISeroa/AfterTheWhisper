// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/TDPlayerController.h"
#include "UI/Widgets/TDW_AmmoWidget.h"
#include "UI/Widgets/TDReloadBarWidget.h"
#include "UI/Widgets/TDHitMarkerWidget.h"
#include "UI/Widgets/TDCrosshairWidget.h"

ATDPlayerController::ATDPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	CachedMouseWorldLocation = FVector::ZeroVector;
}

void ATDPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Warning, TEXT("[PlayerController] Game input mode initialized"));
#endif
}

void ATDPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

#if !UE_BUILD_SHIPPING
	bool bDebugShouldLogCrosshair = false;
	DebugCrosshairLogAccum += DeltaTime;
	if (DebugCrosshairLogAccum >= 1.f)
	{
		DebugCrosshairLogAccum = 0.f;
		bDebugShouldLogCrosshair = true;
	}
#endif

	FVector WorldOrigin, WorldDir;
	if (!DeprojectMousePositionToWorld(WorldOrigin, WorldDir))
		return;

	const FVector Start = WorldOrigin;
	const FVector End = Start + WorldDir * 50000.f;

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(MouseAim), false);
	Params.AddIgnoredActor(GetPawn());

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params) && Hit.bBlockingHit)
	{
		CachedMouseWorldLocation = Hit.ImpactPoint;
	}

	if (!AmmoWidget && !ReloadBarWidget && !HitMarkerWidget && !CrosshairWidget)
	{
#if !UE_BUILD_SHIPPING
		if (bDebugShouldLogCrosshair)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Crosshair] PlayerTick: CrosshairWidget=NULL (no widgets bound, early return)"));
		}
#endif
		return;
	}

	float MouseX = 0.f, MouseY = 0.f;
	const bool bGotMousePos = GetMousePosition(MouseX, MouseY);
	if (bGotMousePos)
	{
		if (AmmoWidget)
		{
			AmmoWidget->SetPositionInViewport(FVector2D(MouseX, MouseY) + MouseOffset, true);
		}

		if (ReloadBarWidget)
		{
			ReloadBarWidget->SetPositionInViewport(FVector2D(MouseX, MouseY) + ReloadWidgetOffset, true);
		}

		if (HitMarkerWidget)
		{
			HitMarkerWidget->SetPositionInViewport(FVector2D(MouseX, MouseY) + HitMarkerWidgetOffset, true);
		}

		if (CrosshairWidget)
		{
			CrosshairWidget->SetPositionInViewport(FVector2D(MouseX, MouseY) + CrosshairWidgetOffset, true);
		}
	}

#if !UE_BUILD_SHIPPING
	if (bDebugShouldLogCrosshair)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[Crosshair] PlayerTick: CrosshairWidget=%s GetMousePosition=%s MouseX=%.1f MouseY=%.1f Offset=(%.1f, %.1f) SetPositionInViewport called=%s"),
			CrosshairWidget ? TEXT("Valid") : TEXT("NULL"),
			bGotMousePos ? TEXT("true") : TEXT("false"),
			MouseX, MouseY,
			CrosshairWidgetOffset.X, CrosshairWidgetOffset.Y,
			(bGotMousePos && CrosshairWidget) ? TEXT("true") : TEXT("false"));
	}
#endif
}

void ATDPlayerController::SetAmmoWidget(UTDW_AmmoWidget* InWidget)
{
	AmmoWidget = InWidget;
}

void ATDPlayerController::SetReloadBarWidget(UTDReloadBarWidget* InWidget)
{
	ReloadBarWidget = InWidget;
}

void ATDPlayerController::SetHitMarkerWidget(UTDHitMarkerWidget* InWidget)
{
	HitMarkerWidget = InWidget;
}

void ATDPlayerController::SetCrosshairWidget(UTDCrosshairWidget* InWidget)
{
	CrosshairWidget = InWidget;

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Warning, TEXT("[Crosshair] SetCrosshairWidget received widget=%s"),
		InWidget ? *InWidget->GetName() : TEXT("NULL"));
#endif
}
