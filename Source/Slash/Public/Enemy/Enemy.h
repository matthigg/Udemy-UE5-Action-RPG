// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Perception/PawnSensingComponent.h"
#include "Enemy.generated.h"

//class UWidgetComponent;
class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:

	AEnemy();
	virtual void Tick(float DeltaTime) override;
	void CheckPatrolTarget();
	void CheckCombatTarget();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;
	virtual void Destroyed() override;

	//UPROPERTY(BlueprintReadOnly)
	//EDeathPose DeathPose = EDeathPose::EDP_Alive;

	UPROPERTY(BlueprintReadOnly, Category = "Death", meta = (AllowPrivateAccess = "true"))
	EDeathPose DeathPose = EDeathPose::EDP_Alive;


protected:

	virtual void BeginPlay() override;
	virtual void Die() override;

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
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 140.f;

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

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
};
