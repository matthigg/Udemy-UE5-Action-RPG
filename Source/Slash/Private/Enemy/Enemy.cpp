// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Slash/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIController.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "NavigationPath.h"
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
}




void AEnemy::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("***** Die *****"));
	UE_LOG(LogTemp, Warning, TEXT("======================================"))

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		//UE_LOG(LogTemp, Warning, TEXT("SUCCESS: AnimInstance and DeathMontage are valid! Playing now."));
		//UE_LOG(LogTemp, Warning, TEXT("======================================"));

		// Randomly select which attack animation to play
		const int32 Selection = FMath::RandRange(0, 2);
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			SectionName = FName("Death1");
			DeathPose = EDeathPose::EDP_Death1;
			break;
		case 1:
			SectionName = FName("Death2");
			DeathPose = EDeathPose::EDP_Death2;
			break;
		case 2:
			SectionName = FName("Death3");
			DeathPose = EDeathPose::EDP_Death3;

			break;
		default:
			break;
		}

		UE_LOG(LogTemp, Warning, TEXT("Selected DeathPose: %s"), *UEnum::GetValueAsString(DeathPose))
		UE_LOG(LogTemp, Warning, TEXT("======================================"))

		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(3.f);
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	UE_LOG(LogTemp, Warning, TEXT("***** GetHit *****"));
	UE_LOG(LogTemp, Warning, TEXT("======================================"));

	//DRAW_SPHERE_COLOR(ImpactPoint, FColor::Magenta);

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}

	if (Attributes && Attributes->IsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		Die();
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint
		);
	}

	if (HitParticles)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Spawning hit particles at location: %s"), *ImpactPoint.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("=============================================="));

		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			HitParticles,
			ImpactPoint,
			FRotator(0.f),
			true
		);
	}

}



float AEnemy::TakeDamage(
	float DamageAmount, 
	FDamageEvent const& DamageEvent, 
	AController* EventInstigator, 
	AActor* DamageCauser
)
{
	if (Attributes && HealthBarWidget)
	{
		Attributes->ReceiveDamage(DamageAmount);
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}

	CombatTarget = EventInstigator->GetPawn();
	EnemyState = EEnemyState::EES_Chasing;
	MoveToTarget(CombatTarget);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;

	return DamageAmount;
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
	MoveRequest.SetAcceptanceRadius(15.f);

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
	UE_LOG(LogTemp, Warning, TEXT("***** PawnSeen *****"));

	if (EnemyState == EEnemyState::EES_Attacking || EnemyState == EEnemyState::EES_Chasing) return;

	// We're using actor tags instead of casting actors to a specific class because we want to be able 
	// to use this sensing functionality for both the player and the AI's allies, and we don't want to 
	// have to cast to multiple classes. By using actor tags, we can simply check if the seen pawn has 
	// the "Player" tag or the "Ally" tag, and if it does, we can set it as the combat target. This 
	// allows us to easily extend this functionality to other types of actors in the future without 
	// having to modify the code.
	if (SeenPawn->ActorHasTag(FName("Echo")) || SeenPawn->ActorHasTag(FName("Ally")))
	{
		// Prevent enemy from walking its patrol path while chasing the player
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		CombatTarget = SeenPawn;

		if (EnemyState != EEnemyState::EES_Attacking)
		{
			EnemyState = EEnemyState::EES_Chasing;
			MoveToTarget(CombatTarget);

			UE_LOG(LogTemp, Warning, TEXT("Enemy has seen a pawn with tag 'Player' or 'Ally' and is now chasing it!"));
		}
	}
	else
	{

	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		CombatTarget = nullptr;
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(false);
		}

		// Outside combat radius, lose interest in target and return to 
		EnemyState = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = 125.f;
		MoveToTarget(PatrolTarget);
		UE_LOG(LogTemp, Warning, TEXT("Enemy has lost sight of the combat target and is now patrolling!"));
	}
	else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
	{
		// Outside attack range, chase character
		EnemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		MoveToTarget(CombatTarget);
		UE_LOG(LogTemp, Warning, TEXT("Enemy is chasing player!"));
	}
	else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking)
	{
		// Inside attack range, attack character
		EnemyState = EEnemyState::EES_Attacking;
		//GetCharacterMovement()->StopMovementImmediately();
		// TODO: Play attack montage and apply damage
		UE_LOG(LogTemp, Warning, TEXT("Enemy is attacking!"));


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

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

