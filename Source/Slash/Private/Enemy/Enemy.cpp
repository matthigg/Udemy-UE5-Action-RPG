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

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
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

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
	// It's useful when calculating dot products for 2 vectors to both be unit vectors.
	// ImpactPoint - GetActorLocation() by itself is not a normalized vector, but you
	// can use *.GetSafeNormal() to normalize it.

	// Returns a normalized vector
	const FVector Forward = GetActorForwardVector();

	// Lower impact point to the enemy's actor location Z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);

	// ImpactPoint - GetActorLocation() is not initially a normalized vector, but we can 
	// use *.GetSafeNormal() to normalize it
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// DotProduct takes 2 vectors and returns a scalar
	// Forward * ToHit = |Forward||ToHit| * cos(theta)
	// [Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);

	// Take the inverse cosine (arccosine) of cos(theta) to get theta. FMath::ACos() 
	// returns the angle in radians.
	double Theta = FMath::Acos(CosTheta);

	// Convert radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	// CrossProduct returns a vector (as opposed to DotProduct which returns a scalar).
	// Unreal Engine uses the left-hand rule when determine whether the result of a
	// cross product is positive or negative (|A||B| is usually positive and |B||A|
	// is usually negative, but it's the opposite in Unreal Engine).
	// Positive = getting hit from the right, negative = getting hit from the left.
	// We're specifically checking the value of Z.
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0) {
		Theta *= -1.f;
	}

	// Since the angle for FromBack is -135 < theta < 135, instead of actually checking
	// that we can check forward, left, and right, and if it's not one of those then it
	// must be back.
	FName Section("FromBack");

	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= -45.f && Theta < -135.f)
	{
		Section = FName("FromRight");
	}

	PlayHitReactMontage(Section);

	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta: %f"), Theta));
	//}

	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 5.f, FColor::Blue, 5.f);
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

	return DamageAmount;
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	//UE_LOG(LogTemp, Warning, TEXT("PlayHitReactMontage SectionName: %s"), *SectionName.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("=============================="));

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		//UE_LOG(LogTemp, Warning, TEXT("SUCCESS: AnimInstance and HitReactMontage are valid! Playing now."));
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("ERROR: AnimInstance is %s | HitReactMontage is %s"),
		//AnimInstance ? TEXT("Valid") : TEXT("NULL"),
		//HitReactMontage ? TEXT("Valid") : TEXT("NULL"));
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
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckCombatTarget();
	CheckPatrolTarget();
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

