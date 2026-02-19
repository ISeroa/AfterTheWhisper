#include "Weapon/TDWeaponBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Weapon/Data/TDWeaponPresetDA.h"

ATDWeaponBase::ATDWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	GripRoot = CreateDefaultSubobject<USceneComponent>(TEXT("GripRoot"));
	GripRoot->SetupAttachment(Root);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(GripRoot);
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATDWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	AmmoInMag = MagazineSize;
	AmmoReserve = 60; //TODO. move to inventory
}

void ATDWeaponBase::SetPartsFromPreset(UTDWeaponPresetDA* Preset, bool bClearMissingSlots)
{
#if !UE_BUILD_SHIPPING
	ensureMsgf(Preset != nullptr, TEXT("Weapon Preset is null. Did you forget to assign DefaultWeaponPreset on the character?"));
#endif

	CurrentPreset = Preset;

	if (!Preset)
	{
		if (bClearMissingSlots)
		{
			Parts.Empty();
			ApplyParts();
		}
		return;
	}

	if (Preset->FrameMesh && BaseMesh)
	{
		BaseMesh->SetStaticMesh(Preset->FrameMesh);
	}

	HandSocketName = Preset->Grip.HandSocketName;
	if (GripRoot)
	{
		GripRoot->SetRelativeTransform(Preset->Grip.RelativeToHand);
	}

	TArray<FWeaponPartSpec> Specs;
	Specs.Reserve(Preset -> Parts.Num());

	TSet<FName> KeepSlots;

	for (const FTDWeaponPartEntry& E : Preset->Parts)
	{
		const UTDWeaponPartDA* Part = E.Part;
		if (!Part) continue;
		
		FWeaponPartSpec S;
		S.Slot = TDSlotToName(Part->Slot);
		if (S.Slot.IsNone()) continue;

		S.Mesh = Part->Mesh;
		S.AttachSocket = Part->AttachSocketName;

		S.RelativeTransform = E.bOverrideOffset ? E.OverrideRelativeToSocket : Part->RelativeOffset;

		Specs.Add(S);
		KeepSlots.Add(S.Slot);
	}

	SetParts(Specs);

	if (bClearMissingSlots)
	{
		ClearAllPartsNotIn(KeepSlots);
	}
}

void ATDWeaponBase::SetParts(const TArray<FWeaponPartSpec>& InParts)
{
	Parts.Empty();
	for (const FWeaponPartSpec& P : InParts)
	{
		if (P.Slot.IsNone()) continue;
		Parts.Add(P.Slot, P);
	}
	ApplyParts();
}

void ATDWeaponBase::SetPart(const FWeaponPartSpec& InPart)
{
	if (InPart.Slot.IsNone()) return;
	Parts.Add(InPart.Slot, InPart);
	ApplyParts();
}

void ATDWeaponBase::ApplyParts()
{
	for (const TPair<FName, FWeaponPartSpec>& Kvp : Parts)
	{
		const FName Slot = Kvp.Key;
		const FWeaponPartSpec& Spec = Kvp.Value;

		UStaticMeshComponent* Comp = GetOrCreatePartComp(Slot);
		if (!Comp) continue;

		Comp->SetStaticMesh(Spec.Mesh);

		if (!Spec.Mesh)
		{
			Comp->SetVisibility(false, true);
			continue;
		}

		//attach socket
		if (!Spec.AttachSocket.IsNone() && BaseMesh && BaseMesh->DoesSocketExist(Spec.AttachSocket))
		{
			Comp->AttachToComponent(BaseMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Spec.AttachSocket);
			Comp->SetRelativeTransform(Spec.RelativeTransform);
		}
		else
		{
			Comp->AttachToComponent(BaseMesh, FAttachmentTransformRules::KeepRelativeTransform);
			Comp->SetRelativeTransform(Spec.RelativeTransform);
		}
		Comp->SetVisibility(true, true);
	}
}

void ATDWeaponBase::ClearAllPartsNotIn(const TSet<FName>& KeepSlots)
{
	for (auto& Pair : PartComps)
	{
		const FName Slot = Pair.Key;
		UStaticMeshComponent* Comp = Pair.Value;
		if (!Comp) continue;

		if (!KeepSlots.Contains(Slot))
		{
			Comp->SetStaticMesh(nullptr);
			Comp->SetVisibility(false, true);
		}
	}
}

void ATDWeaponBase::ClearPart(FName Slot)
{
	if (Slot.IsNone()) return;

	Parts.Remove(Slot);

	if (UStaticMeshComponent** Found = PartComps.Find(Slot))
	{
		if (UStaticMeshComponent* Comp = *Found)
		{
			Comp->SetStaticMesh(nullptr);
			Comp->SetVisibility(false, true);
		}
	}
}

void ATDWeaponBase::NotifyReloadFinished()
{
	if (!IsReloading())
	{
		return;
	}

	FinishReload();

	GetWorldTimerManager().ClearTimer(Timerhandle_Reload);

	EndReloadUI();
	EndReloadState();
}

void ATDWeaponBase::CancelReload()
{
	if (!bIsReloading)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(Timerhandle_Reload);

	EndReloadUI();
	EndReloadState();
}

void ATDWeaponBase::RequestReload()
{
	StartReload();
}

UStaticMeshComponent* ATDWeaponBase::GetOrCreatePartComp(FName Slot)
{
	if (Slot.IsNone()) return nullptr;

	if (UStaticMeshComponent** Found = PartComps.Find(Slot))
	{
		return *Found;
	}

	const FName CompName(*FString::Printf(TEXT("Part_%s"), *Slot.ToString()));
	UStaticMeshComponent* NewComp = NewObject<UStaticMeshComponent>(this, CompName);
	NewComp->RegisterComponent();
	NewComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NewComp->SetVisibility(false, true);

	NewComp->AttachToComponent(BaseMesh, FAttachmentTransformRules::KeepRelativeTransform);

	PartComps.Add(Slot, NewComp);
	return NewComp;
}

UStaticMeshComponent* ATDWeaponBase::FindMuzzleProviderComp() const
{
	//find from priority socket
	if (!MuzzlePrioritySlot.IsNone())
	{
		if (UStaticMeshComponent* const* Found = PartComps.Find(MuzzlePrioritySlot))
		{
			UStaticMeshComponent* C = *Found;
			if (C && C->GetStaticMesh() && C->DoesSocketExist(MuzzleSocketName))
			{
				return C;
			}
		}
	}

	//find from BaseMesh
	if (BaseMesh && BaseMesh->GetStaticMesh() && BaseMesh->DoesSocketExist(MuzzleSocketName))
	{
		return BaseMesh;
	}

	//fallback
	return BaseMesh;
}

FVector ATDWeaponBase::GetMuzzleLocation() const
{
	UStaticMeshComponent* MuzzleComp = FindMuzzleProviderComp();
	if (!MuzzleComp) return GetActorLocation();

	if (MuzzleComp->GetStaticMesh() && MuzzleComp->DoesSocketExist(MuzzleSocketName))
	{
		return MuzzleComp->GetSocketLocation(MuzzleSocketName);
	}

	return MuzzleComp -> GetComponentLocation();
}


void ATDWeaponBase::SetTriggerHeld(bool bHeld)
{
	bTriggerHeld = bHeld;

	if (bTriggerHeld)
	{
		FireOnce();
		StartFireLoop();
	}
	else
	{
		StopFireLoop();
	}
}


void ATDWeaponBase::StartFireLoop()
{
	if (GetWorldTimerManager().IsTimerActive(Timerhandle_FireLoop)) return;

	const  float Interval = (FireRate > 0.f) ? (1.f / FireRate) : 0.1f;
	GetWorldTimerManager().SetTimer(
		Timerhandle_FireLoop,
		this,
		&ATDWeaponBase::FireOnce,
		Interval,
		true
	);
}

void ATDWeaponBase::StopFireLoop()
{
	GetWorldTimerManager().ClearTimer(Timerhandle_FireLoop);
}

void ATDWeaponBase::BeginReloadState()
{
	bIsReloading = true;
}

void ATDWeaponBase::EndReloadState()
{
	bIsReloading = false;
}

void ATDWeaponBase::BeginReloadUI(float Duration)
{
	bShowReloadIndicator = (AmmoInMag == 0);

	if (bShowReloadIndicator)
	{
		OnReloadUIStart.Broadcast(Duration);
	}
}

void ATDWeaponBase::EndReloadUI()
{
	if (bShowReloadIndicator)
	{
		OnReloadUIStop.Broadcast();
	}
	bShowReloadIndicator = false;
}

void ATDWeaponBase::OnReloadTimerFinished()
{
	NotifyReloadFinished();
}

bool ATDWeaponBase::CanFire() const
{
	return !bIsReloading && FireRate> 0.f && AmmoInMag > 0;
}



FVector ATDWeaponBase::GetShotDirection() const
{
	const FVector Muzzle = GetMuzzleLocation();
	FVector Dir = (AimTarget - Muzzle).GetSafeNormal();
	if (SpreadDeg > 0.f)
	{
		const float HalfRad = FMath::DegreesToRadians(SpreadDeg * 0.5f);
		Dir = FMath::VRandCone(Dir, HalfRad);
	}
	return Dir;
}

void ATDWeaponBase::NotifyAmmoChanged()
{
	UE_LOG(LogTemp, Warning, TEXT("[Weapon] NotifyAmmoChanged this=%s Ammo=%d/%d"),
		*GetName(), AmmoInMag, MagazineSize);

	OnAmmoChanged.Broadcast(AmmoInMag, MagazineSize);
}

void ATDWeaponBase::StartReload()
{
	if (bIsReloading) return;
	if (AmmoInMag >= MagazineSize) return;
	if (AmmoReserve == 0) return;

	BeginReloadState();
	BeginReloadUI(ReloadTime);

	GetWorldTimerManager().SetTimer(
		Timerhandle_Reload,
		this,
		&ATDWeaponBase::OnReloadTimerFinished,
		ReloadTime,
		false
	);
}

void ATDWeaponBase::FinishReload()
{
	const int32 Need = MagazineSize - AmmoInMag;

	const int32 Load = FMath::Min(Need, AmmoReserve);

	AmmoInMag += Load;
	AmmoReserve -= Load;

	NotifyAmmoChanged();
	EndReloadUI();
	EndReloadState();
}


void ATDWeaponBase::FireOnce()
{
	UE_LOG(LogTemp, Warning, TEXT("Ammo=%d Reloading=%d"), AmmoInMag, bIsReloading);

	if (!CanFire())
	{
		if (!bIsReloading && AmmoInMag == 0)
		{
			StartReload();
		}
		return;
	}

	AmmoInMag = FMath::Max(AmmoInMag - 1, 0);
	NotifyAmmoChanged();

	UWorld* World = GetWorld();
	AActor* OwnerActor = GetOwner();
	if (!World || !OwnerActor) return;

	const FVector Start = GetMuzzleLocation();
	const FVector Dir = GetShotDirection();
	const FVector End = Start + Dir * Range;

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(WeaponShot), false, OwnerActor);
	Params.AddIgnoredActor(OwnerActor);
	Params.AddIgnoredActor(this);

	const bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, Params);


#if !UE_BUILD_SHIPPING
	if (bDebugShot)
	{
		DrawDebugLine(World, Start, bHit ? Hit.ImpactPoint : End, bHit ? FColor::Green : FColor::Red, false, 0.05f, 0, 1.f);
		if (bHit) DrawDebugSphere(World, Hit.ImpactPoint, 6.f, 8, FColor::Green, false, 0.05f);
	}

	UE_LOG(LogTemp, Warning, TEXT("[Weapon] Hit=%s Damage=%.1f"),
		Hit.GetActor() ? *Hit.GetActor()->GetName() : TEXT("None"),
		Damage);

#endif

	if (bHit && Hit.GetActor() && Hit.GetActor() != OwnerActor)
	{
		UGameplayStatics::ApplyPointDamage(
			Hit.GetActor(),
			Damage,
			Dir,
			Hit,
			OwnerActor->GetInstigatorController(),
			OwnerActor,
			nullptr
		);
	}

}
