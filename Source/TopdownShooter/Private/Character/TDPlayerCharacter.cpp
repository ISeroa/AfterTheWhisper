// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TDPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Core/TDPlayerController.h"
#include "Weapon/TDWeaponBase.h"

// Sets default values
ATDPlayerCharacter::ATDPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

    GetCharacterMovement()->MaxWalkSpeed = 600.0f;

    AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ATDPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    GetCharacterMovement()->MaxWalkSpeed = 300.f;

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

        /*CurrentWeapon = GetWorld()->SpawnActor<ATDWeaponBase>(DefaultWeaponClass);
        if (CurrentWeapon)
        {
            CurrentWeapon->SetOwner(this);
            CurrentWeapon->SetInstigator(this);
            CurrentWeapon->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        }*/
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
    if (CurrentWeapon)
    {
        CurrentWeapon->SetAimTarget(SmoothedAimPoint);
    }
}

void ATDPlayerCharacter::OnFirePressed()
{
    if (CurrentWeapon) CurrentWeapon->SetTriggerHeld(true);
}

void ATDPlayerCharacter::OnFireReleased()
{
    if (CurrentWeapon) CurrentWeapon->SetTriggerHeld(false);
}

void ATDPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ATDPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ATDPlayerCharacter::MoveRight);

    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATDPlayerCharacter::OnFirePressed);
    PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATDPlayerCharacter::OnFireReleased);
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