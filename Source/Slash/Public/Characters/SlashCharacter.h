// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;




protected:
	virtual void BeginPlay() override;

	/**
	* Callbacks for input
	*/

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
	virtual void Attack() override;

	/**
	* Play montage functions
	*/

	virtual void AttackEnd() override;
	virtual bool CanAttack() override;

	void PlayEquipMontage(const FName& SectionName);
	bool CanDisarm();
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

private:
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	// Note: if Category is a single word then you don't need 2x quotes
	//UPROPERTY(VisibleAnywhere, Category = Hair)
	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;





	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;

public:
	// The "inline" keyword is used for single-line functions, which are slightly optimized vs
	// multi-line functions. However, by default, C++ can choose whether or not it actually 
	// inlines the function. You can override this behavior by using the FORCEINLINE macro
	// 
	//inline void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; 
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	// Adding const in front of the function body ensures that this function cannot change
	// anything
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};
