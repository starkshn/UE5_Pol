// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "MyCharacter.h"

UMyAnimInstance::UMyAnimInstance()
{
	CurrentPawnSpeed = 0.f;
	IsInAir = false;
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	if (!IsDead)
	{
		if (::IsValid(Pawn))
		{
			CurrentPawnSpeed = Pawn->GetVelocity().Size();
			CharacterRef = Cast<AMyCharacter>(Pawn);
			if (CharacterRef)
				IsInAir = CharacterRef->GetMovementComponent()->IsFalling();
		}
	}
}
