// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AW2.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class AW2_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UMyAnimInstance();
	
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	
	private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Characater, Meta = (AllowPrivateAccess = true))
	class AMyCharacter* CharacterRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsDead;
};
