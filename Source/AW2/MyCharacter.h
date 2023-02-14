// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AW2.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FOnPressedVault, bool);

UCLASS()
class AW2_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	FOnPressedVault ValutPressed;

public:
	enum class ControlMode
	{
		GTA,
		DAIBLO,
	};

public:
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);

	void ViewChange();
	void RunP();
	void RunR();
	void Jump();
	void Vault();

public:
	void SetControlMode(ControlMode Mode);

public:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* SK_Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = true))
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimInstance, meta = (AllowPrivateAccess = true))
	class UMyAnimInstance* MyAnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = VaultSystem, meta = (AllowPrivateAccess = true))
	class UVaultSystem* VaultSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ClimbSystem, meta = (AllowPrivateAccess = true))
	class UClimbSystem* ClimbSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Run, meta = (AllowPrivateAccess = true))
	bool IsRunning;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Vault, meta = (AllowPrivateAccess = true))
	bool IsVaulting;


private:
	ControlMode		CurrentControlMode = ControlMode::GTA;
	FVector			DirectionToMove = FVector::ZeroVector;
	float			ArmLengthTo = 0.0f;
	FRotator		ArmRotationTo = FRotator::ZeroRotator;
	float			ArmLengthSpeed = 0.f;
	float			ArmRotationSpeed = 0.f;
};
