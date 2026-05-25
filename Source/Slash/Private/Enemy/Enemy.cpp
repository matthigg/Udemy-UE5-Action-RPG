// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Slash/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

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

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	UE_LOG(LogTemp, Warning, TEXT("***** GetHit *****"));
	UE_LOG(LogTemp, Warning, TEXT("======================================"));

	//DRAW_SPHERE_COLOR(ImpactPoint, FColor::Magenta);

	DirectionalHitReact(ImpactPoint);

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
		UE_LOG(LogTemp, Warning, TEXT("Spawning hit particles at location: %s"), *ImpactPoint.ToString());
		UE_LOG(LogTemp, Warning, TEXT("=============================================="));

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

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayHitReactMontage SectionName: %s"), *SectionName.ToString());
	UE_LOG(LogTemp, Warning, TEXT("=============================="));

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("SUCCESS: AnimInstance and HitReactMontage are valid! Playing now."));
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: AnimInstance is %s | HitReactMontage is %s"),
		AnimInstance ? TEXT("Valid") : TEXT("NULL"),
		HitReactMontage ? TEXT("Valid") : TEXT("NULL"));
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

