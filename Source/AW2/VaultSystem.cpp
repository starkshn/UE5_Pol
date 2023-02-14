// Fill out your copyright notice in the Description page of Project Settings.


#include "VaultSystem.h"
#include "MyCharacter.h"

UVaultSystem::UVaultSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	CanVault = false;
}


void UVaultSystem::BeginPlay()
{
	Super::BeginPlay();
	
	auto Pawn = GetOwner();
	MyCharacterRef = Cast<AMyCharacter>(Pawn);

	MyCharacterRef->ValutPressed.BindLambda
	(
		[this](bool Value) -> void 
		{
			if (Value)
			{
				if (CanVault == true) return;

				StartVault();
				// EndVault();
			}
		}
	);
}

void UVaultSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UVaultSystem::StartVault()
{	
	// ##################################################
	// 높이 계산
	CalVaultObjectHeight();
	VaultHeightState = GetVaultObjectHeight();
	DrawAndResetHeight();

	if (VaultHeightState == VaultHeight::Cant) return;
	
	// ###################################################
	// 깊이 계산
	
	// LastIndex가 2보다 작거나 같으면 Thin1
	// LastIndex가 3보다 크거나 같고 6보다 작거나 같으면 thin2
	// LastIndex가 6보다 크거나 같고 7보다 작거나 같으면 Thick
	switch (VaultHeightState)
	{
		case VaultHeight::Slide:
		{
			// Start, Loop, End Location 필요하다
			ABLOG(Warning, TEXT("Switch Slide"));
			int32 li = CalVaultObjectDepth();
			
		}
		break;
		case VaultHeight::Low:
		{
			int32 li = CalVaultObjectDepth();

			// Low인 경우 너무 두껍다면 올라간다.
			VaultDepthState = GetVaultObjectDpeth(li);

			switch (VaultDepthState)
			{
				case VaultDepth::Cant:
				{
					ABLOG_S(Error);
					StartLocation = FVector::ZeroVector;
					LoopLocation = FVector::ZeroVector;
					EndLocation = FVector::ZeroVector;
				}
				break;
				case VaultDepth::Thin1:
				{
					ABLOG(Warning, TEXT("Low And Thin1"));
					SetEndLocation(VaultDepthState);
				}
				break;
				case VaultDepth::Thin2:
				{
					ABLOG(Warning, TEXT("Low And Thin2"));
					SetEndLocation(VaultDepthState);
				}
				break;
				case VaultDepth::Thick:
				{
					ABLOG(Warning, TEXT("Low And Thick"));
					SetEndLocation(VaultDepthState);
				}
				break;
				case VaultDepth::CantToThick:
				{
					ABLOG(Warning, TEXT("Low And TooThick You Have to Climb"));
				}
				break;
			}
		}
		break;
		case VaultHeight::High:
		{
			// Start, Loop, End Location  필요하다
			ABLOG(Warning, TEXT("Switch High"));
			int32 li = CalVaultObjectDepth();

			// High인 경우 너무 두껍다면 올라간다.
			VaultDepthState = GetVaultObjectDpeth(li);

			switch (VaultDepthState)
			{
				case VaultDepth::Cant:
				{
					ABLOG_S(Error);
					StartLocation = FVector::ZeroVector;
					LoopLocation = FVector::ZeroVector;
					EndLocation = FVector::ZeroVector;
				}
				break;
				case VaultDepth::Thin1:
				{
					ABLOG(Warning, TEXT("High And Thin1"));
				}
				break;
				case VaultDepth::Thin2:
				{
					ABLOG(Warning, TEXT("High And Thin2"));
				}
				break;
				case VaultDepth::Thick:
				{
					ABLOG(Warning, TEXT("High And Thick"));
				}
				case VaultDepth::CantToThick:
				{
					ABLOG(Warning, TEXT("High And TooThick You Have to Climb"));
				}
				break;
			}
		}
		break;
		case VaultHeight::HighClimb:
		{
			// StartLocation만 필요하다
			ABLOG(Warning, TEXT("Switch HighClimb"));
			int32 li = CalVaultObjectDepth();

			// 너무 두껍든 아니든 그냥 위로 올라간다.
			VaultDepthState = GetVaultObjectDpeth(li);
		}
		break;
	}
}

void UVaultSystem::EndVault()
{
	VaultHeights.Empty();
	VaultHeightState = VaultHeight::Cant;
	HitIndex = 0;
	NotHitIndex = 0;
	FirstHitFlag = false;
	HeightHitLocation = FVector::ZeroVector;
	VaultDepthState = VaultDepth::Cant;

	StartLocation = FVector::ZeroVector;
	LoopLocation = FVector::ZeroVector;
	EndLocation = FVector::ZeroVector;
}

bool UVaultSystem::Check()
{
	if (VaultHeightState == VaultHeight::Cant || VaultDepthState == VaultDepth::Cant || StartLocation == FVector::ZeroVector || LoopLocation == FVector::ZeroVector || EndLocation == FVector::ZeroVector) return false;
	return true;
}

void UVaultSystem::CalVaultObjectHeight()
{
	for (int32 i = 1; i <= 7; ++i)
	{
		FHitResult HitResult;
		FCollisionQueryParams Params(NAME_None, false, MyCharacterRef);

		float Range = 200.f;

		FVector Start = MyCharacterRef->GetActorLocation();
		Start.Z = Start.Z - MyCharacterRef->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 40.f * i;
		FVector End = Start + MyCharacterRef->GetActorForwardVector() * Range;

		bool bResult = GetWorld()->LineTraceSingleByChannel
		(
			OUT HitResult,
			Start,
			End,
			ECollisionChannel::ECC_GameTraceChannel2,
			Params
		);

		FColor DrawColor;
		if (bResult)
		{
			DrawColor = FColor::Green;
			VaultHeights.Add(i);

			DrawDebugLine
			(
				GetWorld(), Start, End, DrawColor, false, 3.f, 2, 1.f
			);
			HeightHitLocation = HitResult.Location;
		}
		else
		{
			DrawColor = FColor::Red;
			VaultHeights.Add(0);
			DrawDebugLine
			(
				GetWorld(), Start, End, DrawColor, false, 3.f, 2, 1.f
			);
		}
	}
	return;
}

void UVaultSystem::DrawAndResetHeight()
{
	VaultHeights.Empty();
	HitIndex = 0;
	NotHitIndex = 0;
	NotHitLoop = 0;
	FirstHitFlag = false;

	DrawDebugSphere
	(
		GetWorld(), HeightHitLocation, 10.f, 12, FColor::Red, false, 5.f, 2, 1.f
	);
}

int32 UVaultSystem::CalVaultObjectDepth()
{
	// 캐릭터 앞에 물체가 무조건 있는 경우임.
	FVector Start = HeightHitLocation + (MyCharacterRef->GetActorForwardVector() * 10.f);
	Start.Z = Start.Z + 100.f;

	bool FirstHit = false;
	bool FirstNotHit = false;

	FVector StartLocation2;
	FVector LoopLocation2;

	int32	LastIndex = 0;

	// 안 맞을 때까지 반복함.
	for (int32 i = 0; i <= 10; ++i)
	{
		FHitResult HitResult;
		FCollisionQueryParams Params(NAME_None, false, MyCharacterRef);

		float Range = 120.f;

		FVector StartPos = Start + ( i * 30.f * MyCharacterRef->GetActorForwardVector() );
		FVector EndPos = StartPos + (MyCharacterRef->GetActorUpVector() * -1 * Range);
		
		bool bResult = GetWorld()->LineTraceSingleByChannel
		(
			OUT HitResult,
			StartPos,
			EndPos,
			ECollisionChannel::ECC_GameTraceChannel2,
			Params
		);

		FColor DrawColor;
		if (bResult)
		{
			if (FirstHit == false)
			{
				FirstHit = true;
				DrawColor = FColor::Green;
				StartLocation2 = HitResult.Location;
				StartLocation = StartLocation2;
			}
			else
			{
				DrawColor = FColor::Green;
			}

			// 너무 두꺼운 경우 종료함
			if (i >= 8)
			{
				ABLOG(Warning, TEXT("Too Thick! Can't Vault!"));
				return 8;
			}

			LoopLocation2 = HitResult.Location;
			LoopLocation = LoopLocation2;
			LastIndex = i;

			DrawDebugLine
			(
				GetWorld(), StartPos, EndPos, DrawColor, false, 3.f, 2, 1.f
			);
		}
		else
		{
			// 왔다는 것은 안맞았다는 말이다.
			break;
		}
	}

	ABLOG(Warning, TEXT("LastIndex : %d"), LastIndex);
	ABLOG(Warning, TEXT("You Can Vault!"));

	DrawDebugSphere
	(
		GetWorld(), StartLocation2, 10.f, 12, FColor::Yellow, false, 5.f, 2, 1.f
	);

	DrawDebugSphere
	(
		GetWorld(), LoopLocation2, 10.f, 12, FColor::Cyan, false, 5.f, 2, 1.f
	);

	return LastIndex;
}

UVaultSystem::VaultDepth UVaultSystem::GetVaultObjectDpeth(int32 LastIndex)
{
	if (LastIndex >= 8)
	{
		return VaultDepth::CantToThick;
	}
	else if (LastIndex >= 0 && LastIndex <= 1)
	{
		// Thin1
		return VaultDepth::Thin1;
	}
	else if (LastIndex >= 2 && LastIndex <= 3)
	{
		// Thin2
		return VaultDepth::Thin2;
	}
	else if (LastIndex >= 4 && LastIndex <= 7)
	{
		// Thick
		return VaultDepth::Thick;
	}
	else
	{
		return VaultDepth::Cant;
	}
}

void UVaultSystem::SetEndLocation(VaultDepth VaultDepthState2)
{
	switch (VaultHeightState)
	{
		case VaultHeight::Low:
		{
			switch (VaultDepthState2)
			{
				case VaultDepth::Thin1:
				{
					CalLowVaultEndLocation(0);
					FinalState = FinalVaultState::LowThin1;
				}
				break;
				case VaultDepth::Thin2:
				{
					CalLowVaultEndLocation(1);
					FinalState = FinalVaultState::LowThin2;
				}
				break;
				case VaultDepth::Thick:
				{
					CalLowVaultEndLocation(2);
					FinalState = FinalVaultState::LowThick;
				}
				break;
				case VaultDepth::CantToThick:
				{
					CalLowVaultCantToThickEndLocation();
					FinalState = FinalVaultState::LowCantToThick;
				}
				break;
				case VaultDepth::Cant:
				{
					FinalState = FinalVaultState::None;
				}
				break;
			}
		}
		break;
	}

	if (Check() == false) return;
	CanVault = true;
}

void UVaultSystem::CalLowVaultEndLocation(int32 Value)
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, MyCharacterRef);

	float Range = 400.f;

	FVector Start = LoopLocation + (MyCharacterRef->GetActorForwardVector() * (150.f + (Value * 70)));
	FVector End = Start + (MyCharacterRef->GetActorUpVector() * -1 * Range);

	bool bResult = GetWorld()->LineTraceSingleByChannel
	(
		OUT HitResult,
		Start,
		End,
		ECollisionChannel::ECC_GameTraceChannel2,
		Params
	);

	FColor DrawColor;

	if (bResult) DrawColor = FColor::Green;
	else DrawColor = FColor::Red;

	if (bResult)
	{
		DrawDebugLine
		(
			GetWorld(), Start, End, DrawColor, false, 3.f, 2, 1.f
		);

		EndLocation = HitResult.Location;

		DrawDebugSphere(GetWorld(), EndLocation, 12.f, 12, DrawColor, false, 10.f, 2, 1.f);
	}
	else
	{
		// 왔다는 것은 안맞았다는 말이다.
		DrawDebugLine
		(
			GetWorld(), Start, End, DrawColor, false, 3.f, 2, 1.f
		);
	}
	
}

void UVaultSystem::CalLowVaultCantToThickEndLocation()
{
	EndLocation = StartLocation;
	
	DrawDebugSphere(GetWorld(), EndLocation, 30.f, 12, FColor::Black, false, 10.f, 2, 2.f);
}

UVaultSystem::VaultHeight UVaultSystem::GetVaultObjectHeight()
{
	VaultHeight VH;
	FirstHitFlag = false;

	for (auto i = VaultHeights.begin(); i != VaultHeights.end(); ++i)
	{
		++NotHitLoop;

		if (*i != 0)
		{
			// 맞은 경우
			HitIndex = *i;
			if (HitIndex == 1)
			{
				FirstHitFlag = true;
			}
		}
		else if (*i == 0)
		{
			// 맞지 않은 경우
			if (FirstHitFlag == false)
			{
				if (NotHitLoop <= 3)
				{
					NotHitIndex = NotHitLoop;
				}
			}
		}
	}

	if (HitIndex >= 6)
	{
		ABLOG(Warning, TEXT("Cant Vault"));
		VH = VaultHeight::Cant;
	}

	ABLOG(Warning, TEXT("Hit Index : %d"), HitIndex);
	ABLOG(Warning, TEXT("NotHit Index : %d"), NotHitIndex);

	// 첫번째 부터 맞지 않은 경우
	if (NotHitIndex != 0 && NotHitIndex <= 2 && HitIndex > 2)
	{
		ABLOG(Warning, TEXT("Object Slide"));
		return VaultHeight::Slide;
	}
	else if (NotHitIndex != 0 && NotHitIndex == 3 && HitIndex >= 4)
	{
		ABLOG(Warning, TEXT("Object High"));
		return VaultHeight::High;
	}

	// 첫번째 부터 맞은 경우 (슬라이딩 아님)
	if (FirstHitFlag == true && HitIndex <= 2)
	{
		ABLOG(Warning, TEXT("Object Low"));
		return VaultHeight::Low;
	}
	else if (FirstHitFlag == true && HitIndex >= 3 && HitIndex <= 4)
	{
		ABLOG(Warning, TEXT("Object High"));
		return VaultHeight::High;
	}
	else if (FirstHitFlag == true && HitIndex == 5)
	{
		ABLOG(Warning, TEXT("Object High Climb"));
		return VaultHeight::HighClimb;
	}
	else
	{
		ABLOG(Warning, TEXT("Object Cant"));
		return VaultHeight::Cant;
	}
}

void UVaultSystem::ResetVault()
{
	CanVault = false;
	MyCharacterRef->IsVaulting = false;
}

