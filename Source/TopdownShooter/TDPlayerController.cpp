// Fill out your copyright notice in the Description page of Project Settings.


#include "TDPlayerController.h"

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

	//AActor* HitActor = Hit.GetActor();
	//if (GEngine)
	//{
	//	const FString Name = HitActor ? HitActor->GetName() : TEXT("None");
	//	GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Yellow, FString::Printf(TEXT("Hit: %s"), *Name));
	//}

}
