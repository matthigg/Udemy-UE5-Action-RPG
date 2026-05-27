// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"

class AWeapon;

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::Attack()
{
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::Die()
{
}

void ABaseCharacter::PlayAttackMontage()
{
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	// This explicitly calls the engine's reflection system for this specific enum type
	//UE_LOG(LogTemp, Warning, TEXT("CollisionEnabled: %s"), *StaticEnum<ECollisionEnabled::Type>()->GetValueAsString(CollisionEnabled));
	//UE_LOG(LogTemp, Warning, TEXT("================================"));

	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);

		// This resets actors to ignore which is used to prevent multiple actor hits per weapon swing
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
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



