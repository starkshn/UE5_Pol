// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "MyAnimInstance.h"
#include "VaultSystem.h"
#include "ClimbSystem.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = GetCapsuleComponent();
	CapsuleComp = GetCapsuleComponent();
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	VaultSystem = CreateDefaultSubobject<UVaultSystem>(TEXT("VaultSystem"));
	ClimbSystem = CreateDefaultSubobject<UClimbSystem>(TEXT("ClimbSystem"));


	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -70.f), FRotator(0.f, -90.f, 0.f));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MY_Mesh(TEXT("/Script/Engine.SkeletalMesh'/Game/AW2Files/Character/RootCharacter/Root_Ch44_nonPBR.Root_Ch44_nonPBR'"));
	if (MY_Mesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MY_Mesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> MY_Anim(TEXT("/Script/Engine.AnimBlueprint'/Game/AW2Files/BlueprintClasses/BP_RootMyCharacterAnim.BP_RootMyCharacterAnim_C'"));
	if (MY_Anim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(MY_Anim.Class);
	}

	GetCharacterMovement()->JumpZVelocity = 400.0f;

	SetControlMode(ControlMode::GTA);
	ArmLengthSpeed = 3.f;
	ArmRotationSpeed = 10.f;
	IsRunning = 0;

	IsVaulting = false;
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	ABLOG(Warning, TEXT("MyCharacter BeginPlay!"));
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case ControlMode::DAIBLO:
	{
		FRotator Rotator = FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed);
		SpringArm->SetRelativeRotation(Rotator);
	}
	break;
	}

	switch (CurrentControlMode)
	{
	case ControlMode::DAIBLO:
	{
		if (DirectionToMove.SizeSquared() > 0.f)
		{
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			AddMovementInput(DirectionToMove);
		}
	}
	break;
	}
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AMyCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AMyCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMyCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMyCharacter::Turn);

	PlayerInputComponent->BindAction(TEXT("ViewChange"), IE_Pressed, this, &AMyCharacter::ViewChange);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &AMyCharacter::RunP);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &AMyCharacter::RunR);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AMyCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Vault"), IE_Pressed, this, &AMyCharacter::Vault);
	PlayerInputComponent->BindAction(TEXT("Vault"), IE_Released, this, &AMyCharacter::Vault);
}

void AMyCharacter::UpDown(float NewAxisValue)
{
	
	switch (CurrentControlMode)
	{
		case ControlMode::GTA:
		{
			AddMovementInput(FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::X), NewAxisValue);
		}
		break;
		case ControlMode::DAIBLO:
		{
			DirectionToMove.X = NewAxisValue;
		}
		break;
	}
}

void AMyCharacter::LeftRight(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
		case ControlMode::GTA:
		{
			AddMovementInput(FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::Y), NewAxisValue);
		}
		break;
		case ControlMode::DAIBLO:
		{
			DirectionToMove.Y = NewAxisValue;
		}
		break;
	}
}

void AMyCharacter::LookUp(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
		case ControlMode::GTA:
		{
			AddControllerPitchInput(NewAxisValue);
		}
		break;
	}
}

void AMyCharacter::Turn(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
		case ControlMode::GTA:
		{
			AddControllerYawInput(NewAxisValue);
		}
		break;
	}
}

void AMyCharacter::ViewChange()
{
	switch (CurrentControlMode)
	{
	case ControlMode::GTA:
	{
		GetController()->SetControlRotation(GetActorRotation());
		SetControlMode(ControlMode::DAIBLO);
	}
	break;
	case ControlMode::DAIBLO:
	{
		GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
		SetControlMode(ControlMode::GTA);
	}
	break;
	}
}

void AMyCharacter::RunP()
{
	IsRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
}

void AMyCharacter::RunR()
{
	IsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
}

void AMyCharacter::Jump()
{
	bPressedJump = true;
}

void AMyCharacter::Vault()
{
	if (IsVaulting == false)
	{
		ABLOG(Warning, TEXT("Start Vault in MyCharacater!"));
		IsVaulting = true;
		ValutPressed.ExecuteIfBound(true);
	}
}

void AMyCharacter::SetControlMode(ControlMode Mode)
{
	CurrentControlMode = Mode;

	switch (Mode)
	{
		case ControlMode::GTA:
		{
			// SpringArm->TargetArmLength = 450.f;
			// SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
			ArmLengthTo = 450.f;

			SpringArm->bUsePawnControlRotation = true;
			SpringArm->bInheritPitch = false;
			SpringArm->bInheritYaw = true;
			SpringArm->bInheritRoll = true;
			SpringArm->bDoCollisionTest = true;
			bUseControllerRotationYaw = false;

			// 캐릭터가 부드럽게 회전할 수 있도록
			GetCharacterMovement()->bOrientRotationToMovement = true;
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

			GetCharacterMovement()->MaxWalkSpeed = 200.f;
		}
		break;
		case ControlMode::DAIBLO:
		{
			// SpringArm->TargetArmLength = 800.f;
			// SpringArm->SetRelativeRotation(FRotator(-45, 0.f, 0.f));
			ArmLengthTo = 800.f;
			ArmRotationTo = FRotator(-45.f, 0.f, 0.f);

			SpringArm->bUsePawnControlRotation = false;
			SpringArm->bInheritPitch = false;
			SpringArm->bInheritYaw = false;
			SpringArm->bInheritRoll = false;
			SpringArm->bDoCollisionTest = false;

			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

			GetCharacterMovement()->MaxWalkSpeed = 200.f;
		}
		break;
	}
}
