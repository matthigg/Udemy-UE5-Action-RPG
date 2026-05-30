// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Slash/DebugMacros.h"

class AWeapon;

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsAlive() && Hitter) DirectionalHitReact(Hitter->GetActorLocation());
	else Die();

	PlayHitSound(ImpactPoint);
	SpawnParticles(ImpactPoint);
}

void ABaseCharacter::Attack()
{
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::Die()
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
	UE_LOG(LogTemp, Warning, TEXT("%s: Calculating hit direction..."), *GetName());

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
	//FName Section("FromBack");

	//if (Theta >= -45.f && Theta < 45.f)
	//{
	//	Section = FName("FromFront");
	//}
	//else if (Theta >= -135.f && Theta < -45.f)
	//{
	//	Section = FName("FromLeft");
	//}
	//else if (Theta >= 45.f && Theta < 135.f)
	////else if (Theta >= -45.f && Theta < -135.f)
	//{
	//	Section = FName("FromRight");
	//}

	//PlayHitReactMontage(Section);

	FName Section("FromBack");

	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}

	UE_LOG(LogTemp, Warning, TEXT("%s: Hit from %s (Theta: %.2f degrees)"), *GetName(), *Section.ToString(), Theta);

	PlayHitReactMontage(Section);

}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint
		);
	}
}

void ABaseCharacter::SpawnParticles(const FVector& ImpactPoint)
{

	if (HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			HitParticles,
			ImpactPoint,
			FRotator(0.f),
			true
		);
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayAttackMontage()
{
	//// ─── LOG THE MONTAGE SECTIONS ARRAY ───
	//UE_LOG(LogTemp, Warning, TEXT("=== %s: Printing AttackMontageSections (%d items) ==="), *GetName(), AttackMontageSections.Num());

	//// If the array is empty, let yourself know immediately
	//if (AttackMontageSections.Num() == 0)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("%s: AttackMontageSections array is EMPTY! Check your character instance details panel."), *GetName());
	//}
	//else
	//{
	//	// Iterate through each element in the array
	//	for (int32 i = 0; i < AttackMontageSections.Num(); ++i)
	//	{
	//		// Note: Assuming AttackMontageSections contains FName. 
	//		// If it's already an FString, remove the '.ToString()' part.
	//		FString SectionNameStr = AttackMontageSections[i].ToString();

	//		UE_LOG(LogTemp, Display, TEXT("   Section [%d]: %s"), i, *SectionNameStr);
	//	}
	//}

	// Original functionality remains completely untouched
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	return PlayRandomMontageSection(DeathMontage, DeathMontageSections);
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;

	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);

	return Selection;
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget)
	{
		const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
		FVector ToTarget = (CombatTargetLocation - GetActorLocation()).GetSafeNormal();
		ToTarget *= WarpTargetDistance;
		return CombatTargetLocation + ToTarget;
	}
	return FVector();
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}
