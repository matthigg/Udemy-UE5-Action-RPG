// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationPath.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Slash/DebugMacros.h"
#include "Items/Soul.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetGenerateOverlapEvents(true);

	// Set the mesh and capsule to ignore the camera
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll= false;

	PawnSensingCPP = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingCPP"));
	PawnSensingCPP->SightRadius = 4000.f;
	PawnSensingCPP->SetPeripheralVisionAngle(45.f);

	// ─── ADD THIS LINE TO FIX THE SPAWNING DEAD BUG ───
	DeathPose = EDeathPose::EDP_MAX;
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Cyan, FString::Printf(TEXT("Current State: %d"), (int32)EnemyState));

	if (IsDead()) return;

	// The EnemyState enum order:
	// EES_Patrolling = 0
	// EES_Chasing = 1
	// EES_Attacking = 2
	if (EnemyState > EEnemyState::EES_Patrolling) {
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

float AEnemy::TakeDamage(
	float DamageAmount, 
	FDamageEvent const& DamageEvent, 
	AController* EventInstigator, 
	AActor* DamageCauser
)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();

	if (IsInsideAttackRadius() && !IsAttacking() && !IsEngaged() && !IsDead())
	{
		StartAttackTimer();
	}
	else if (IsOutsideAttackRadius())
	{
		ChaseTarget();
	}

	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (!IsDead()) ShowHealthBar();
	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	StopAttackMontage();

	if (IsInsideAttackRadius())
	{
		if (!IsDead()) StartAttackTimer();
	}
	else if (!IsEngaged())
	{
		//ChaseTarget();
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (PawnSensingCPP) PawnSensingCPP->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	InitializeEnemy();
	Tags.Add(FName("Enemy"));
}

void AEnemy::Die_Implementation()
{
	Super::Die_Implementation();
	EnemyState = EEnemyState::EES_Dead;
	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnSoul();
}

void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();
	if (World && SoulClass && Attributes)
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 125.f);
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation());
		if (SpawnedSoul)
		{
			SpawnedSoul->SetSouls(Attributes->GetSouls());
			SpawnedSoul->SetOwner(this);
		}
	}
}

void AEnemy::Attack()
{
	Super::Attack();
	if (CombatTarget == nullptr) return;
	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	bool bCanAttack =
		IsInsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsDead();
	return bCanAttack;
}

void AEnemy::AttackEnd()
{
	//EnemyState = EEnemyState::EES_NoState;
	EnemyState = EEnemyState::EES_Patrolling;
	CheckCombatTarget();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	HideHealthBar();
	SpawnDefaultWeapon();
}

void AEnemy::CheckCombatTarget()
{
	// Outside combat radius, lose interest in target and return to patrolling
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if (!IsEngaged()) StartPatrolling();
	}
	// Outside attack range, chase character
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();
		if (!IsEngaged()) ChaseTarget();
	}
	// Inside attack range, attack character
	else if (CanAttack())
	{
		StartAttackTimer();
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);

}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		if (DefaultWeapon)
		{
			DefaultWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
			EquippedWeapon = DefaultWeapon;

			// ─── ADD THESE LINES TO FIX THE STUTTERING COLLIDERS ───
			// Forces this enemy character to completely ignore physical collision sweeps with the weapon
			this->MoveIgnoreActorAdd(DefaultWeapon);
		}
	}
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;

	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();

	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr)
	{
		return;
	}

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

	EnemyController->MoveTo(MoveRequest);

}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumberOfPatrolTargets = ValidTargets.Num();

	if (NumberOfPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumberOfPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}

	return nullptr;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget = 
		EnemyState != EEnemyState::EES_Dead	&&
		EnemyState != EEnemyState::EES_Attacking &&
		EnemyState != EEnemyState::EES_Engaged &&
		EnemyState != EEnemyState::EES_Chasing &&

		// We're using actor tags instead of casting actors to a specific class because we want to be able 
		// to use this sensing functionality for both the player and the AI's allies, and we don't want to 
		// have to cast to multiple classes. By using actor tags, we can simply check if the seen pawn has 
		// the "Player" tag or the "Ally" tag, and if it does, we can set it as the combat target. This 
		// allows us to easily extend this functionality to other types of actors in the future without 
		// having to modify the code.
		SeenPawn->ActorHasTag(FName("Echo"));

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

