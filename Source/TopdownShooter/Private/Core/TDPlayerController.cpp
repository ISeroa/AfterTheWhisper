// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/TDPlayerController.h"
#include "UI/Widgets/TDW_AmmoWidget.h"

ATDPlayerController::ATDPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	CachedMouseWorldLocation = FVector::ZeroVector;
}

void ATDPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

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

	if (!AmmoWidget) return;

	float MouseX, MouseY;
	if (GetMousePosition(MouseX, MouseY))
	{
		AmmoWidget->SetPositionInViewport(FVector2D(MouseX, MouseY) + MouseOffset, true);
	}
}

void ATDPlayerController::SetAmmoWidget(UTDW_AmmoWidget* InWidget)
{
	AmmoWidget = InWidget;
}