// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashCharacter.h"
#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharactermovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GroomComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"

ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll= false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

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

	// This tag is used in Enemy.cpp via SeenPawn->ActorHasTag(FName("Echo")
	Tags.Add(FName("Echo"));
	
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

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	ActionState = EActionState::EAS_HitReaction;
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
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		EquipWeapon(OverlappingWeapon);
	}
	else {
		if (CanDisarm()) Disarm();
		else if (CanArm()) Arm();
	}
}

void ASlashCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::Attack()
{
	Super::Attack();

	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	WeaponToEquip->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = WeaponToEquip;
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
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

void ASlashCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::AttachWeaponToHand()
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

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
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
