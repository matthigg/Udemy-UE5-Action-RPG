// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStartCPP = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStartCPP->SetupAttachment(GetRootComponent());

	BoxTraceEndCPP = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEndCPP->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
	//ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
	//if (SlashCharacter)
	//{

		// 1. Turn off physics collisions so the player doesn't get stuck
		if (ItemMesh)
		{
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		}
		if (SphereCPP)
		{
			SphereCPP->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SphereCPP->SetCollisionResponseToAllChannels(ECR_Ignore);
		}

		AttachMeshToSocket(InParent, InSocketName);
		ItemState = EItemState::EIS_Equipped;
		if (EquipSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
		}

	//}
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	// EAttachmentRules is a "scoped" enum; regular enums usually just have the enum type followed by the enum name, e.g.
	// enum MyEnum, but a "scoped" enum also has the class type, e.g. enum class MyEnum. Scoped enums require you to 
	// qualify the enum with the scope resolution operator, e.g. MyEnum::SomeEnum
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	//ItemMesh->AttachToComponent(SlashCharacter->GetMesh(), TransformRules, FName("RightHandSocket"));
	ItemMesh->AttachToComponent(InParent, TransformRules, FName(InSocketName));
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::OnBoxOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	UE_LOG(LogTemp, Warning, TEXT("OnBoxOverlap"));
	UE_LOG(LogTemp, Warning, TEXT("======================================"));

	const FVector Start = BoxTraceStartCPP->GetComponentLocation();
	const FVector End = BoxTraceEndCPP->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	FHitResult BoxHit;

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		FVector(5.f, 5.f, 5.f),
		BoxTraceStartCPP->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		BoxHit,
		true
	);

	// ==========================================
	// 1. LOGGING OUT BOXHIT DETAILS
	// ==========================================
	if (BoxHit.bBlockingHit)
	{
		// Logs general hit info: bone name hit, component hit, and impact location vectors
		UE_LOG(LogTemp, Warning, TEXT("BoxHit Registered! Component: %s | Bone: %s | Impact Point: %s"),
			*BoxHit.GetComponent()->GetName(),
			*BoxHit.BoneName.ToString(),
			*BoxHit.ImpactPoint.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("BoxTrace missed everything."));
	}

	// ==========================================
	// 2. LOGGING OUT BOXHIT.GETACTOR()
	// ==========================================
	if (BoxHit.GetActor())
	{
		// Safely extract the hit actor's name
		FString HitActorName = BoxHit.GetActor()->GetName();

		UE_LOG(LogTemp, Warning, TEXT("BoxHit.GetActor() Found: %s"), *HitActorName);
		UE_LOG(LogTemp, Warning, TEXT("==============================="));

		IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
		if (HitInterface)
		{
			HitInterface->GetHit(BoxHit.ImpactPoint);
		}
	}
}













//void AWeapon::OnBoxOverlap(
//	UPrimitiveComponent* OverlappedComponent, 
//	AActor* OtherActor, UPrimitiveComponent* 
//	OtherComp, 
//	int32 OtherBodyIndex, 
//	bool bFromSweep, 
//	const FHitResult& SweepResult
//)
//{
//	UE_LOG(LogTemp, Warning, TEXT("OnBoxOverlap"));
//	UE_LOG(LogTemp, Warning, TEXT("======================================"));
//
//
//	// GetComponentLocation() gets from the world/global location, GetRelativeLocation() gets from the local location
//	const FVector Start = BoxTraceStartCPP->GetComponentLocation();
//	const FVector End = BoxTraceEndCPP->GetComponentLocation();
//
//	// Note: a TArray is a class with a dynamically adjusted array, which is nice considering C++ doesn't offer that
//	// by default. Additionally, TArray is a template class, which is why we have to specify the type to store. Also,
//	// TArrays typically aren't pointers, which is why we're access its Add method via dot notation, e.g.
//	// ActorsToIgnore.Add(...);
//	TArray<AActor*> ActorsToIgnore;
//	ActorsToIgnore.Add(this);
//	FHitResult BoxHit;
//	UKismetSystemLibrary::BoxTraceSingle(
//		this,
//		Start,
//		End,
//		FVector(5.f, 5.f, 5.f),
//		BoxTraceStartCPP->GetComponentRotation(),
//		ETraceTypeQuery::TraceTypeQuery1,
//		false,
//		ActorsToIgnore,
//		EDrawDebugTrace::ForDuration,
//		BoxHit, // Parameter name is FHitResult &OutHits, so the argument is passed by reference
//		true
//	);
//	if (BoxHit.GetActor())
//	{
//		UE_LOG(LogTemp, Warning, TEXT("BoxHit.GetActor()"));
//		UE_LOG(LogTemp, Warning, TEXT("==============================="));
//
//		// Since the AEnemy class in Enemy.h inherits IHitInterface, it "is" of type IHitInterface
//		// (in addition to ACharacter).
//		IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
//		if (HitInterface)
//		{
//			HitInterface->GetHit(BoxHit.ImpactPoint);
//		}
//
//	}
//}
