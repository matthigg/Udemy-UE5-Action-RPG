// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Interfaces/HitInterface.h" // Need to include this since we're inheriting from it


#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	// ECollisionEnabled::Type is an enum type
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	void DirectionalHitReact(const FVector& ImpactPoint);


protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	virtual void Attack();
	virtual bool CanAttack();

	virtual void Die();


	/**
	* Play montage functions
	*/

	virtual void PlayAttackMontage();
	void PlayHitReactMontage(const FName& SectionName);


	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;

	/**
	* Animation montages
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	/**
	* Components
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;


	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "VisualEffects")
	UParticleSystem* HitParticles;

private:	


};
