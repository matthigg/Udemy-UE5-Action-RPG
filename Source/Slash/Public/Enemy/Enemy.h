// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h" // Need to include this since we're inheriting from it
#include "Characters/CharacterTypes.h"
#include "Perception/PawnSensingComponent.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UAttributeComponent;
class UWidgetComponent;
class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class SLASH_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:

	AEnemy();
	virtual void Tick(float DeltaTime) override;
	void CheckPatrolTarget();
	void CheckCombatTarget();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	void DirectionalHitReact(const FVector& ImpactPoint);

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	//UPROPERTY(BlueprintReadOnly)
	//EDeathPose DeathPose = EDeathPose::EDP_Alive;

	UPROPERTY(BlueprintReadOnly, Category = "Death", meta = (AllowPrivateAccess = "true"))
	EDeathPose DeathPose = EDeathPose::EDP_Alive;


protected:

	virtual void BeginPlay() override;
	void Die();

	/**
	* Play montage functions
	*/

	void PlayHitReactMontage(const FName& SectionName);

	//virtual void PossessedBy(AController* NewController) override;

	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);


private:

	/**
	* Components
	*/

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingCPP;

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	/**
	* Animation montages
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "VisualEffects")
	UParticleSystem* HitParticles;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	/**
	* Navigation
	*/

	UPROPERTY()
	class AAIController* EnemyController;

	// Current Patrol Target
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI Navigation", meta = (AllowPrivateAccess = true))
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;


	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();
};
