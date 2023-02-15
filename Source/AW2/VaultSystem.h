// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AW2.h"
#include "Components/ActorComponent.h"
#include "VaultSystem.generated.h"

UENUM(BlueprintType)
enum class FinalVaultState : uint8
{
	LowThin1		UMETA(DisplayName = "LowThin1"),
	LowThin2		UMETA(DisplayName = "LowThin2"),
	LowThick		UMETA(DisplayName = "LowThick"),
	LowCantToThick	UMETA(DisplayName = "LowCantToThick"),
	HighThin1		UMETA(DisplayName = "HighThin1"),
	HighThin2		UMETA(DisplayName = "HighThin2"),
	HighThick		UMETA(DisplayName = "HighThick"),
	HighCantToThick UMETA(DisplayName = "HighCantToThick"),
	None			UMETA(DisplayName = "None"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AW2_API UVaultSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UVaultSystem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	enum class VaultHeight
	{
		Low,
		High,
		HighClimb,
		Slide,
		Cant,
	};

	enum class VaultDepth
	{
		Thin1,
		Thin2,
		Thick,
		CantToThick,
		Cant
	};

public:
	void		StartVault();
	void		EndVault();
	bool		Check();
	void		CalVaultObjectHeight();
	void		DrawAndResetHeight();

	int32		CalVaultObjectDepth();
	VaultDepth	GetVaultObjectDpeth(int32 LastIndex);

	void		SetEndLocation(VaultDepth VaultDepthState);
	void		CalLowVaultEndLocation(int32 Value);
	void		CalLowVaultCantToThickEndLocation();
	
public:
	VaultHeight GetVaultObjectHeight();
	
	UFUNCTION(BlueprintCallable, Category = VaultFunc)
	void ResetVault();
	
public:
	// Start -> M1 -> M2 -> Loop -> End
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = true))
	class AMyCharacter* MyCharacterRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VaultLocation, Meta = (AllowPrivateAccess = true))
	FVector StartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VaultLocation, Meta = (AllowPrivateAccess = true))
	FVector Middle1Location;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VaultLocation, Meta = (AllowPrivateAccess = true))
	FVector Middle2Location;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VaultLocation, Meta = (AllowPrivateAccess = true))
	FVector LoopLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VaultLocation, Meta = (AllowPrivateAccess = true))
	FVector EndLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Vault, Meta = (AllowPrivateAccess = true))
	bool CanVault;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VaultState, Meta = (AllowPrivateAccess = true))
	FinalVaultState FinalState;

private:
	TArray<int32>	VaultHeights;
	VaultHeight		VaultHeightState = VaultHeight::Cant;


	// Valut Height
	int32			HitIndex = 0;
	int32			NotHitIndex = 0;
	int32			NotHitLoop = 0;
	bool			FirstHitFlag = false;

	// Vault Depth
	FVector			HeightHitLocation;
	VaultDepth		VaultDepthState = VaultDepth::Cant;
};
