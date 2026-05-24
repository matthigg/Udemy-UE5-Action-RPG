// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharactermovementComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"

ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll= false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ASlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASlashCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ASlashCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ASlashCharacter::LookUp);

	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &ASlashCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &ASlashCharacter::Attack);

}

void ASlashCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
	}
}

void ASlashCharacter::MoveForward(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;

	if (Controller && (Value != 0.f))
	{
		// Find out which way is forward
		const FRotator ControlRotation = GetControlRotation();
		// Note: YawRotation is an object variable, here we are assigning values to it via "direct initialization"
		// where these values are passed to its constructor.
		//const FRotator YawRotation = FRotator(0.f, ControlRotation.Yaw, 0.f); // This is equivalent to the line below
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		// GetUnitAxis returns a unit vector, e.g. a normalized vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// Here we're using Direction as the unit vector, and Value as a scalar value
		AddMovementInput(Direction, Value);
	}
}

void ASlashCharacter::MoveRight(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;

	if (Controller && (Value != 0.f))
	{
		// Find out which way is right
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ASlashCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ASlashCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ASlashCharacter::EKeyPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("E KEY PRESSED"));
	UE_LOG(LogTemp, Warning, TEXT("=============================="));

	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"));
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		OverlappingItem = nullptr;
		EquippedWeapon = OverlappingWeapon;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("ELSE"));
		UE_LOG(LogTemp, Warning, TEXT("=============================="));

		if (CanDisarm()) {

			UE_LOG(LogTemp, Warning, TEXT("CAN DISARM"));
			UE_LOG(LogTemp, Warning, TEXT("=============================="));

			PlayEquipMontage(FName("Unequip"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
		else if (CanArm()) {

			UE_LOG(LogTemp, Warning, TEXT("CAN ARM"));
			UE_LOG(LogTemp, Warning, TEXT("=============================="));

			PlayEquipMontage(FName("Equip"));
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
}

void ASlashCharacter::Attack()
{
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

bool ASlashCharacter::CanAttack()
{
	return
		ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanDisarm()
{
	const bool x = ActionState == EActionState::EAS_Unoccupied;
	const bool y = CharacterState != ECharacterState::ECS_Unequipped;

	UE_LOG(LogTemp, Warning, TEXT("CanDisarm ActionState: %s"), *UEnum::GetValueAsString(ActionState));
	UE_LOG(LogTemp, Warning, TEXT("CanDisarm CharacterState: %s"), *UEnum::GetValueAsString(CharacterState));
	//UE_LOG(LogTemp, Warning, TEXT("CanDisarm ActionState: %s"), x ? TEXT("true") : TEXT("false"));
	//UE_LOG(LogTemp, Warning, TEXT("CanDisarm CharacterState: %s"), y ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("CanDisarm: %s"), x && y ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("=============================="));

	return
		ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm()
{
	const bool x = ActionState == EActionState::EAS_Unoccupied;
	const bool y = CharacterState == ECharacterState::ECS_Unequipped;

	UE_LOG(LogTemp, Warning, TEXT("CanDisarm ActionState: %s"), *UEnum::GetValueAsString(ActionState));
	UE_LOG(LogTemp, Warning, TEXT("CanDisarm CharacterState: %s"), *UEnum::GetValueAsString(CharacterState));
	//UE_LOG(LogTemp, Warning, TEXT("CanArm ActionState %s"), x ? TEXT("true") : TEXT("false"));
	//UE_LOG(LogTemp, Warning, TEXT("CanArm CharacterState: %s"), y ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("CanArm: %s"), x && y ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("=============================="));

	return
		ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

void ASlashCharacter::Disarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ASlashCharacter::FinishEquipping()
{
	UE_LOG(LogTemp, Warning, TEXT("FinishEquipping"));
	UE_LOG(LogTemp, Warning, TEXT("=============================="));

	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);

		// Randomly select which attack animation to play
		const int32 Selection = FMath::RandRange(0, 2);
		FName SectionName = FName();

		switch (Selection)
		{
		case 0:
			SectionName = FName("Attack1");
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		case 2:
			SectionName = FName("Attack3");
			break;
		default:
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void ASlashCharacter::PlayEquipMontage(FName SectionName)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayEquipMontage SectionName %s"), *SectionName.ToString());
	UE_LOG(LogTemp, Warning, TEXT("=============================="));

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}



