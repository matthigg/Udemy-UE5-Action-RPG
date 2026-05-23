// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemMovementRotation.h"
#include "Slash/DebugMacros.h"

// Sets default values
AItemMovementRotation::AItemMovementRotation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItemMovementRotation::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemMovementRotation::Tick(float DeltaTime)
{
	// DeltaTime in units of sec/frame, so multiplying by MovementRate (in units of cm/s) gives us the distance to move in this frame
	Super::Tick(DeltaTime);

	// MovementRate in units of cm/sec, so multiplying by DeltaTime (in seconds) gives us the distance to move in this frame
	float MovementRate = 50.f;
	float RotationRate = 45.f;

	// cm/sec * sec/frame = cm/frame
	float DeltaX = MovementRate * DeltaTime;
	float DeltaYaw = RotationRate * DeltaTime;

	// Multiplying by DeltaTime makes the movement frame rate independent, 
	// so the item will move at the same speed regardless of frame rate
	AddActorWorldOffset(FVector(DeltaX, 0.f, 0.f));
	AddActorWorldRotation(FRotator(0.f, DeltaYaw, 0.f));

	DRAW_SPHERE_SINGLE_FRAME(GetActorLocation());
	DRAW_VECTOR_SINGLE_FRAME(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f);

}

