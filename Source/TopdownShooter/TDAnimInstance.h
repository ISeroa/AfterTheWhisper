// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TDAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNSHOOTER_API UTDAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	float Speed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	float MoveAngle = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bIsCrouched = false;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bIsMoving = false;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Locomotion|Tuning")
	float  MovingSpeedThreshold = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Locomotion|Tuning")
	float  AngleInterpSpeed = 12.f;

	UPROPERTY(EditDefaultsOnly, Category = "Locomotion|Tuning")
	bool bReturnAngleToZeroWhenStopped =  true;

private:
	TWeakObjectPtr<ACharacter> CachedCharacter;

	float SmoothedAngle = 0.f;
};
