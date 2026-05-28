// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Slash/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIController.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "NavigationPath.h"
#include "Items/Weapons/Weapon.h"
#include "Perception/PawnSensingComponent.h"




// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetGenerateOverlapEvents(true);

	// Set the mesh and capsule to ignore the camera
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

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

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}

	// This grabs the controller whether the enemy was placed or spawned!
	EnemyController = Cast<AAIController>(GetController());

	MoveToTarget(PatrolTarget);
	
	if (PawnSensingCPP)
	{
		PawnSensingCPP->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	UWorld* World = GetWorld();
	if (World)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}




void AEnemy::Die()
{
	EnemyState = EEnemyState::EES_Dead;
	PlayDeathMontage();
	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	ShowHealthBar();

	if (IsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else Die();

	PlayHitSound(ImpactPoint);
	SpawnParticles(ImpactPoint);
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
	ChaseTarget();

	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
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
	MoveRequest.SetAcceptanceRadius(60.f);

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

void AEnemy::Attack()
{
	Super::Attack();

	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	bool bCanAttack = 
		IsInsideAttackRadius() && 
		!IsAttacking() && 
		!IsDead();
	return bCanAttack;
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

int32 AEnemy::PlayDeathMontage()
{
	const int32 Selection = Super::PlayDeathMontage();
	TEnumAsByte<EDeathPose> Pose(Selection);

	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}

	return Selection;
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

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, FMath::RandRange(1.f, 2.f));
	}
}



