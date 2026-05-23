// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemSinusoidalVarsExposedBPs.h"
#include "Slash/DebugMacros.h"

// Sets default values
AItemSinusoidalVarsExposedBPs::AItemSinusoidalVarsExposedBPs()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItemSinusoidalVarsExposedBPs::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemSinusoidalVarsExposedBPs::Tick(float DeltaTime)
{
	// DeltaTime in units of sec/frame, so multiplying by MovementRate (in units of cm/s) gives us the distance to move in this frame
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	// Period = 2 * pi / K, where K is the coefficient of the variable in the sine function. 
	// In our case, K = Speed, so Period = 2*pi / Speed
	float DeltaZ = Amplitude * FMath::Sin(RunningTime * TimeConstant);

	AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));

	DRAW_SPHERE_SINGLE_FRAME(GetActorLocation());
	DRAW_VECTOR_SINGLE_FRAME(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f);

}

