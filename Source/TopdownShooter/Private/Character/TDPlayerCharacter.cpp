// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TDPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/UserWidget.h"
#include "DrawDebugHelpers.h"
#include "Core/TDPlayerController.h"
#include "Weapon/TDWeaponBase.h"
#include "UI/Widgets/TDW_AmmoWIdget.h"
#include "UI/Widgets/TDReloadBarWidget.h"
#include "UI/Widgets/TDPlayerStatusHUD.h"
#include "UI/Widgets/TDHitMarkerWidget.h"
#include "UI/Widgets/TDCrosshairWidget.h"
#include "Components/TDHealthComponent.h"
#include "Components/TDVisionComponent.h"
#include "Components/TDActorVisibilityComponent.h"
#include "Components/TDVisionRendererComponent.h"
#include "Inventory/TDInventoryComponent.h"
#include "Inventory/Data/TDItemDataAsset.h"
#include "Inventory/TDItemPickupActor.h"

// Sets default values
ATDPlayerCharacter::ATDPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

    USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->bUsePawnControlRotation = false; 
    SpringArm->SetUsingAbsoluteRotation(true);
    SpringArm->TargetArmLength = 900.0f;
    SpringArm->SetRelativeRotation(FRotator(-65.0f, 0.0f, 0.0f));
    SpringArm->bInheritYaw = false;
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritRoll = false;
    SpringArm->bDoCollisionTest = false;
    SpringArm->SetUsingAbsoluteRotation(true);

    GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

    UCameraComponent* Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = false;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->bUseControllerDesiredRotation = false;

    AutoPossessPlayer = EAutoReceiveInput::Player0;

    VisionComponent = CreateDefaultSubobject<UTDVisionComponent>(TEXT("VisionComponent"));
    ActorVisibilityComponent = CreateDefaultSubobject<UTDActorVisibilityComponent>(TEXT("ActorVisibilityComponent"));
    VisionRendererComponent = CreateDefaultSubobject<UTDVisionRendererComponent>(TEXT("VisionRendererComponent"));

    InventoryComponent = CreateDefaultSubobject<UTDInventoryComponent>(TEXT("InventoryComponent"));
}

void ATDPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

    if (InventoryComponent)
    {
        InventoryComponent->OnInventoryWeightChanged.AddDynamic(this, &ATDPlayerCharacter::HandleInventoryWeightChanged);
        HandleInventoryWeightChanged(InventoryComponent->GetTotalWeight());
    }
    else
    {
        UpdateMoveSpeed();
    }

    if (DefaultWeaponClass)
    {
        FActorSpawnParameters Params;
        Params.Owner = this;
        Params.Instigator = this;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        CurrentWeapon = GetWorld()->SpawnActor<ATDWeaponBase>(DefaultWeaponClass, Params);
        if (CurrentWeapon)
        {
            CurrentWeapon->SetOwner(this);

            if(DefaultWeaponPreset)
            { 
                CurrentWeapon->SetPartsFromPreset(DefaultWeaponPreset, true);
            }

            CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentWeapon->GetHandSocketName());
        }

    

        UE_LOG(LogTemp, Warning, TEXT("[UI] AmmoWidgetClass=%s"),
            AmmoWidgetClass ? *AmmoWidgetClass->GetName() : TEXT("NULL"));

        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            UE_LOG(LogTemp, Warning, TEXT("[UI] PC=%s IsLocal=%d"),
                *PC->GetName(), PC->IsLocalController());

            AmmoWidget = CreateWidget<UTDW_AmmoWidget>(PC, AmmoWidgetClass);
            UE_LOG(LogTemp, Warning, TEXT("[UI] AmmoWidget=%s"),
                AmmoWidget ? *AmmoWidget->GetName() : TEXT("NULL"));

            if (AmmoWidget)
            {
                AmmoWidget->AddToViewport();
                AmmoWidget->BindWeapon(CurrentWeapon);
                UE_LOG(LogTemp, Warning, TEXT("[UI] AddedToViewport"));
                if (ATDPlayerController* TDPC = Cast<ATDPlayerController>(GetController()))
                {
                    TDPC->SetAmmoWidget(AmmoWidget);
                }
            }

            if (ReloadBarWidgetClass)
            {
                ReloadBarWidget = CreateWidget<UTDReloadBarWidget>(PC, ReloadBarWidgetClass);
                if (ReloadBarWidget)
                {
                    ReloadBarWidget->AddToViewport();
                    ReloadBarWidget->BindWeapon(CurrentWeapon);

                    if (ATDPlayerController* TDPC = Cast<ATDPlayerController>(PC))
                    {
                        TDPC->SetReloadBarWidget(ReloadBarWidget);
                    }
                }
            }

        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[UI] No PlayerController on BeginPlay"));
        }

        UE_LOG(LogTemp, Warning, TEXT("[UI] Bind OnAmmoChanged: Weapon=%s"),
            CurrentWeapon ? *CurrentWeapon->GetName() : TEXT("NULL"));

        if (CurrentWeapon && AmmoWidget)
        {
            AmmoWidget->HandleAmmoChanged(CurrentWeapon->GetAmmoInMag(), CurrentWeapon->GetMagazineSize());
        }

        if (CurrentWeapon)
        {
            CurrentWeapon->OnWeaponFired.AddDynamic(this, &ATDPlayerCharacter::HandleWeaponFired);
        }
    }

    if (StatusHUDClass)
    {
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            StatusHUD = CreateWidget<UTDPlayerStatusHUD>(PC, StatusHUDClass);
            if (StatusHUD)
            {
                StatusHUD->AddToViewport();
                if (HealthComponent)
                {
                    HealthComponent->OnHealthChanged.AddDynamic(this, &ATDPlayerCharacter::HandleHealthChanged);
                    StatusHUD->SetHealth(HealthComponent->CurrentHealth, HealthComponent->MaxHealth);
                }
            }
        }
    }

#if !UE_BUILD_SHIPPING
    UE_LOG(LogTemp, Warning, TEXT("[HitMarker] WidgetClass=%s"),
        HitMarkerWidgetClass ? *HitMarkerWidgetClass->GetName() : TEXT("NULL"));
#endif

    if (HitMarkerWidgetClass)
    {
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            HitMarkerWidget = CreateWidget<UTDHitMarkerWidget>(PC, HitMarkerWidgetClass);
#if !UE_BUILD_SHIPPING
            UE_LOG(LogTemp, Warning, TEXT("[HitMarker] Widget=%s"),
                HitMarkerWidget ? *HitMarkerWidget->GetName() : TEXT("NULL"));
#endif
            if (HitMarkerWidget)
            {
                HitMarkerWidget->AddToViewport();
                HitMarkerWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));

                if (ATDPlayerController* TDPC = Cast<ATDPlayerController>(PC))
                {
                    TDPC->SetHitMarkerWidget(HitMarkerWidget);
                }
            }
        }
    }

    if (CurrentWeapon && HitMarkerWidget)
    {
        CurrentWeapon->OnHitMarker.AddDynamic(this, &ATDPlayerCharacter::HandleHitMarker);
#if !UE_BUILD_SHIPPING
        UE_LOG(LogTemp, Warning, TEXT("[HitMarker] Bind OnHitMarker"));
#endif
    }

#if !UE_BUILD_SHIPPING
    UE_LOG(LogTemp, Warning, TEXT("[Crosshair] CrosshairWidgetClass=%s"),
        CrosshairWidgetClass ? *CrosshairWidgetClass->GetName() : TEXT("NULL"));
#endif

    if (CrosshairWidgetClass)
    {
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            CrosshairWidget = CreateWidget<UTDCrosshairWidget>(PC, CrosshairWidgetClass);
#if !UE_BUILD_SHIPPING
            UE_LOG(LogTemp, Warning, TEXT("[Crosshair] CreateWidget result=%s"),
                CrosshairWidget ? *CrosshairWidget->GetName() : TEXT("NULL"));
#endif

            if (CrosshairWidget)
            {
#if !UE_BUILD_SHIPPING
                UE_LOG(LogTemp, Warning, TEXT("[Crosshair] Before AddToViewport"));
#endif
                CrosshairWidget->AddToViewport();
#if !UE_BUILD_SHIPPING
                UE_LOG(LogTemp, Warning, TEXT("[Crosshair] After AddToViewport"));
#endif

                CrosshairWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
#if !UE_BUILD_SHIPPING
                UE_LOG(LogTemp, Warning, TEXT("[Crosshair] SetAlignmentInViewport(0.5, 0.5) called"));
#endif

                if (ATDPlayerController* TDPC = Cast<ATDPlayerController>(PC))
                {
                    TDPC->SetCrosshairWidget(CrosshairWidget);
#if !UE_BUILD_SHIPPING
                    UE_LOG(LogTemp, Warning, TEXT("[Crosshair] SetCrosshairWidget called on PlayerController"));
#endif
                }
            }
        }
    }
}

void ATDPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
        
    Speed = GetVelocity().Size2D();

    FRotator AimRot = GetControlRotation();
    FRotator ActorRot = GetActorRotation();

    FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(AimRot, ActorRot);

    AimYaw = Delta.Yaw;
    AimPitch = Delta.Pitch;

    FVector RawAimPoint;
    const bool bGotRaw = GetMouseAimPointRaw(RawAimPoint);

    //if Raw not exist, keep last value. if not do nothing
    if (!bGotRaw)
    {
        if (!bHasLastAimPoint) return;
        RawAimPoint = LastAimPoint;
    }
//
    // Deadzone(remove vibration)
    if (bHasLastAimPoint)
    {
        const float DistSq2D = FVector::DistSquared2D(RawAimPoint, LastAimPoint);
        if (DistSq2D < FMath::Square(AimPointDeadZone))
        {
            RawAimPoint = LastAimPoint;
        }
    }

    // AimPoint Smoothing(Noise mitigation)
    if (!bHasLastAimPoint)
    {
        SmoothedAimPoint = RawAimPoint;
        LastAimPoint = RawAimPoint;
        bHasLastAimPoint = true;
    }
    else
    {
        SmoothedAimPoint = FMath::VInterpTo(SmoothedAimPoint, RawAimPoint, DeltaTime, TurnSpeedDegPerSec);
        LastAimPoint = RawAimPoint;
    }

    // Last Rotation is SmoothedAimPoint
    UpdateAimRotationFromPoint(DeltaTime, SmoothedAimPoint);

    Debug_PrintMoveSpeed();

    if (VisionComponent)
    {
        VisionComponent->DrawDebugVision();
    }
    if (CurrentWeapon)
    {
        CurrentWeapon->SetAimTarget(SmoothedAimPoint);
    }

    bool bUpdatedCrosshair = false;
    if (CrosshairWidget && CurrentWeapon)
    {
        CrosshairWidget->BP_UpdateCrosshair(CurrentWeapon->GetSpreadDeg(), false);
        bUpdatedCrosshair = true;
    }

#if !UE_BUILD_SHIPPING
    DebugCrosshairLogAccum += DeltaTime;
    if (DebugCrosshairLogAccum >= 1.f)
    {
        DebugCrosshairLogAccum = 0.f;
        UE_LOG(LogTemp, Warning,
            TEXT("[Crosshair] Tick: CrosshairWidget=%s CurrentWeapon=%s SpreadDeg=%.2f BP_UpdateCrosshair called=%s"),
            CrosshairWidget ? TEXT("Valid") : TEXT("NULL"),
            CurrentWeapon ? TEXT("Valid") : TEXT("NULL"),
            CurrentWeapon ? CurrentWeapon->GetSpreadDeg() : -1.f,
            bUpdatedCrosshair ? TEXT("true") : TEXT("false"));
    }
#endif
}

void ATDPlayerCharacter::OnFirePressed()
{
    if (!CurrentWeapon) return;
    CurrentWeapon->StartFire();
}

void ATDPlayerCharacter::OnFireReleased()
{
    if (!CurrentWeapon) return;
    CurrentWeapon->StopFire();
}

void ATDPlayerCharacter::OnReloadPressed()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->RequestReload();
    }
}

void ATDPlayerCharacter::OnSprintPressed()
{
    bWantsToSprint = true;
    UpdateMoveSpeed();
}

void ATDPlayerCharacter::OnSprintReleased()
{
    bWantsToSprint = false;
    UpdateMoveSpeed();
}

void ATDPlayerCharacter::UpdateMoveSpeed()
{
    const float BaseSpeed = bWantsToSprint ? SprintSpeed : WalkSpeed;
    GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * WeightSpeedMultiplier;
}

float ATDPlayerCharacter::CalculateWeightSpeedMultiplier(float TotalWeight) const
{
    if (MaxPenaltyWeight <= NoPenaltyWeight)
    {
        return (TotalWeight > NoPenaltyWeight) ? MinWeightSpeedMultiplier : 1.f;
    }

    if (TotalWeight <= NoPenaltyWeight)
    {
        return 1.f;
    }

    if (TotalWeight >= MaxPenaltyWeight)
    {
        return MinWeightSpeedMultiplier;
    }

    const float Alpha = (TotalWeight - NoPenaltyWeight) / (MaxPenaltyWeight - NoPenaltyWeight);
    return FMath::Lerp(1.f, MinWeightSpeedMultiplier, Alpha);
}

void ATDPlayerCharacter::HandleInventoryWeightChanged(float NewTotalWeight)
{
    WeightSpeedMultiplier = CalculateWeightSpeedMultiplier(NewTotalWeight);
    UpdateMoveSpeed();

#if !UE_BUILD_SHIPPING
    UE_LOG(LogTemp, Warning,
        TEXT("[InventoryWeight] Total=%.1f NoPenalty=%.1f MaxPenalty=%.1f MinMul=%.2f CurrentMul=%.2f MaxWalkSpeed=%.1f"),
        NewTotalWeight, NoPenaltyWeight, MaxPenaltyWeight, MinWeightSpeedMultiplier, WeightSpeedMultiplier,
        GetCharacterMovement()->MaxWalkSpeed);
#endif
}

void ATDPlayerCharacter::TestAddInventoryItem()
{
    if (!InventoryComponent || !TestInventoryItem) return;
    InventoryComponent->AddItem(TestInventoryItem, 1);
}

void ATDPlayerCharacter::OnInteractPressed()
{
    if (!FocusedPickupActor) return;
    FocusedPickupActor->TryPickup(this);
}

void ATDPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ATDPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ATDPlayerCharacter::MoveRight);

    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATDPlayerCharacter::OnFirePressed);
    PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATDPlayerCharacter::OnFireReleased);
    PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ATDPlayerCharacter::OnReloadPressed);
    PlayerInputComponent->BindAction("TestDamage", IE_Pressed, this, &ATDPlayerCharacter::TestDamage);
    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATDPlayerCharacter::OnSprintPressed);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATDPlayerCharacter::OnSprintReleased);
    PlayerInputComponent->BindAction("TestAddInventoryItem", IE_Pressed, this, &ATDPlayerCharacter::TestAddInventoryItem);
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ATDPlayerCharacter::OnInteractPressed);
}

void ATDPlayerCharacter::MoveForward(float Value)
{
    if (Value == 0.0f) return;
    AddMovementInput(FVector::ForwardVector, Value);
}

void ATDPlayerCharacter::MoveRight(float Value)
{
    if (Value == 0.0f) return;
    AddMovementInput(FVector::RightVector, Value);
}

bool ATDPlayerCharacter::GetMouseAimPointRaw(FVector& OutAimPoint) const
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return false;

    FVector WorldOrigin, WorldDir;
    if (!PC->DeprojectMousePositionToWorld(WorldOrigin, WorldDir))
        return false;

    const FVector TraceStart = WorldOrigin;
    const FVector TraceEnd = WorldOrigin + (WorldDir * AimTraceDistance);

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(AimTrace), false, this);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit, TraceStart, TraceEnd, AimTraceChannel, Params
    );

    Debug_PrintTraceChannel();
    Debug_PrintHit(Hit);
    Debug_DrawTrace(TraceStart, TraceEnd, Hit, bHit);

    FVector AimPoint;

    if (bHit)
    {
        AimPoint = Hit.ImpactPoint;
    }
    else
    {
        if (!bFallbackToAimPlane)
            return false;

        //  WorldDir.Z가 너무 작으면 평면 투영이 튀므로 금지
        if (FMath::Abs(WorldDir.Z) < MinAbsDirZForPlane)
            return false;

        const float PlaneZ = GetActorLocation().Z;
        const float T = (PlaneZ - WorldOrigin.Z) / WorldDir.Z;
        if (T < 0.f) return false;

        AimPoint = WorldOrigin + WorldDir * T;
    }

    // 거리 보정(2D)
    const FVector ActorLoc = GetActorLocation();
    FVector ToAim = AimPoint - ActorLoc;
    ToAim.Z = 0.f;

    const float Dist2D = ToAim.Size();
    if (Dist2D < AimMinDistance)
        return false;

    if (AimMaxDistance > 0.f && Dist2D > AimMaxDistance)
    {
        ToAim = ToAim.GetSafeNormal() * AimMaxDistance;
        AimPoint = ActorLoc + ToAim;
        AimPoint.Z = ActorLoc.Z;
    }

    OutAimPoint = AimPoint;
    return true;
}



void ATDPlayerCharacter::UpdateAimRotationFromPoint(float DeltaTime, const FVector& AimPoint)
{
    FVector Flat = AimPoint - GetActorLocation();
    Flat.Z = 0.f;
    if (Flat.IsNearlyZero()) return;

    const float TargetYaw = Flat.Rotation().Yaw;
    const float CurrentYaw = GetActorRotation().Yaw;

    const float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentYaw, TargetYaw);

    const float MaxStep = TurnSpeedDegPerSec * DeltaTime;

    const float Step = FMath::Clamp(DeltaYaw, -MaxStep, MaxStep);
    
    SetActorRotation(FRotator(0.f, CurrentYaw + Step, 0.f));
}

void ATDPlayerCharacter::TestDamage()
{
    UGameplayStatics::ApplyDamage(this, 10.f, GetController(), this, UDamageType::StaticClass());
}

void ATDPlayerCharacter::HandleWeaponFired()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && FireMontage && !AnimInstance->Montage_IsPlaying(FireMontage))
    {
        AnimInstance->Montage_Play(FireMontage);
    }
}

void ATDPlayerCharacter::HandleHealthChanged(float NewHealth, float Delta)
{
    if (StatusHUD && HealthComponent)
    {
        StatusHUD->SetHealth(NewHealth, HealthComponent->MaxHealth);
    }
}

void ATDPlayerCharacter::HandleHitMarker()
{
#if !UE_BUILD_SHIPPING
    UE_LOG(LogTemp, Warning, TEXT("[HitMarker] HandleHitMarker called. Widget=%s"),
        HitMarkerWidget ? *HitMarkerWidget->GetName() : TEXT("NULL"));
#endif

    if (HitMarkerWidget)
    {
        HitMarkerWidget->BP_ShowHitMarker();
    }
}

void ATDPlayerCharacter::Debug_PrintTraceChannel() const
{
#if !UE_BUILD_SHIPPING
    if (!bDebugAimTrace) return;

    const int32 ChannelValue = (int32)AimTraceChannel;
    const UEnum* EnumPtr = StaticEnum<ECollisionChannel>();
    FString ChannelName = EnumPtr
        ? EnumPtr->GetNameStringByValue(ChannelValue)
        : TEXT("Unknown");

    UE_LOG(LogTemp, Warning,
        TEXT("[AimTrace] Channel: %s (%d)"),
        *ChannelName, ChannelValue
    );
#endif
}

void ATDPlayerCharacter::Debug_PrintHit(const FHitResult& Hit) const
{
#if !UE_BUILD_SHIPPING
    if (!bDebugAimTrace) return;

    if (!Hit.bBlockingHit)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AimTrace] No Hit"));
        return;
    }

    UE_LOG(LogTemp, Warning,
        TEXT("[AimTrace] HitActor=%s | Comp=%s | Loc=%s"),
        Hit.GetActor() ? *Hit.GetActor()->GetName() : TEXT("None"),
        Hit.GetComponent() ? *Hit.GetComponent()->GetName() : TEXT("None"),
        *Hit.ImpactPoint.ToString()
    );
#endif
}

void ATDPlayerCharacter::Debug_PrintMoveSpeed()
{
#if !UE_BUILD_SHIPPING
    if (!bDebugMoveSpeed) return;

    const float CurrentVelocity = GetVelocity().Size2D();
    const float MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

    const FString Msg = FString::Printf(
        TEXT("[MoveSpeed] Vel=%.1f Max=%.1f Walk=%.0f Sprint=%.0f WeightMul=%.2f Sprinting=%d"),
        CurrentVelocity, MaxWalkSpeed, WalkSpeed, SprintSpeed, WeightSpeedMultiplier, bWantsToSprint);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(/*Key=*/ 9001, /*TimeToDisplay=*/ 0.f, FColor::Yellow, Msg);
    }

    DebugMoveSpeedLogAccum += GetWorld()->GetDeltaSeconds();
    if (DebugMoveSpeedLogAccum >= 0.5f)
    {
        DebugMoveSpeedLogAccum = 0.f;
        UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
    }
#endif
}

void ATDPlayerCharacter::Debug_DrawTrace(
    const FVector& Start,
    const FVector& End,
    const FHitResult& Hit,
    bool bHit
) const
{
#if !UE_BUILD_SHIPPING
    if (!bDebugAimTrace) return;
    DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 0.f, 0, 2.f);

    if (bHit)
    {
        DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 18.f, 16, FColor::Green, false, 0.f, 0, 2.f);
    }

    DrawDebugDirectionalArrow(
        GetWorld(),
        GetActorLocation(),
        GetActorLocation() + GetActorForwardVector() * 150.f,
        40.f,
        FColor::Yellow,
        false,
        0.f,
        0,
        3.f
    );
#endif
}