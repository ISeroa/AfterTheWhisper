// Fill out your copyright notice in the Description page of Project Settings.


#include "TDAnimInstance.h"
#include "GameFramework/Character.h"

static  float InterpAngleDegrees(float Current, float Target, float DeltaSeconds, float InterpSpeed)
{
	const float Delta = FMath::FindDeltaAngleDegrees(Current, Target);
	const float Alpha = FMath::Clamp(DeltaSeconds * InterpSpeed, 0.f, 1.f);
	return Current + Delta * Alpha;
}

void UTDAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CachedCharacter = Cast<ACharacter>(TryGetPawnOwner());
	SmoothedAngle = 0.f;
	Speed = 0.f;
	MoveAngle = 0.f;
	bIsMoving = false;
	bIsCrouched = false;
}


void UTDAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ACharacter* Char = CachedCharacter.Get();
	if (!Char)
	{
		Char = Cast<ACharacter>(TryGetPawnOwner());
		CachedCharacter = Char;
	}
	if (!Char)
	{
		Speed = 0.f;
		bIsMoving = false;
		bIsCrouched = false;
		MoveAngle = 0.f;
		return;
	}

	bIsCrouched = Char->bIsCrouched;

	const FVector Vel = Char->GetVelocity();
	const FVector Vel2D(Vel.X, Vel.Y, 0.f);

	const float RawSpeed = Vel2D.Size();
	bIsMoving = (RawSpeed > MovingSpeedThreshold);
	Speed = bIsMoving ? RawSpeed : 0.f;

	if (!bIsMoving)
	{
		if (bReturnAngleToZeroWhenStopped)
		{
			SmoothedAngle = InterpAngleDegrees(SmoothedAngle, 0.f, DeltaSeconds, AngleInterpSpeed);
		}
		MoveAngle = SmoothedAngle;
		return;
	}

	const FVector Dir = Vel2D.GetSafeNormal();
	const FVector Fwd3D = Char->GetActorForwardVector();
	const FVector Fwd(Fwd3D.X, Fwd3D.Y, 0.f);
	const FVector FwdN = Fwd.GetSafeNormal();

	const float Dot = FVector::DotProduct(FwdN, Dir);
	const float CrossZ = FVector::CrossProduct(FwdN, Dir).Z;

	const float TargetAngle = FMath::RadiansToDegrees(FMath::Atan2(CrossZ, Dot));

	SmoothedAngle = InterpAngleDegrees(SmoothedAngle, TargetAngle, DeltaSeconds, AngleInterpSpeed);
	MoveAngle = SmoothedAngle;
}