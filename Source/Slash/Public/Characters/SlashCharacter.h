// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;
class USlashOverlay;
class ASoul;
class ATreasure;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:

	virtual void Tick(float DeltaTime) override;

	/** <AActor> */
	ASlashCharacter();
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;

protected:

	virtual void BeginPlay() override;

	// Callbacks for input
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
	virtual void Attack() override;
	void Dodge();

	// Combat functions
	void EquipWeapon(AWeapon* WeaponToEquip);
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;
	virtual bool CanAttack() override;
	void PlayEquipMontage(const FName& SectionName);
	bool CanDisarm();
	bool CanArm();
	void Disarm();
	void Arm();
	virtual void Die_Implementation() override;
	bool HasEnoughStamina();
	bool IsOccupied();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

private:

	bool IsUnoccupied();
	void InitializeSlashOverlay();
	void SetHUDHealth();

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;

	UPROPERTY()
	USlashOverlay* SlashOverlay;

public:

	// The "inline" keyword is used for single-line functions, which are slightly optimized vs
	// multi-line functions. However, by default, C++ can choose whether or not it actually 
	// inlines the function. You can override this behavior by using the FORCEINLINE macro.
	// Adding const in front of the function body ensures that this function cannot change
	// anything
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
};
