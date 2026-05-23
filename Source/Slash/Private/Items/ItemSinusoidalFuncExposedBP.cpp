// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemSinusoidalFuncExposedBP.h"
#include "Slash/DebugMacros.h"

// Sets default values
AItemSinusoidalFuncExposedBP::AItemSinusoidalFuncExposedBP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItemSinusoidalFuncExposedBP::BeginPlay()
{
	Super::BeginPlay();
	
}

float AItemSinusoidalFuncExposedBP::TransformedSin(float Value)
{
	return Amplitude * FMath::Sin(Value * TimeConstant);
}

float AItemSinusoidalFuncExposedBP::TransformedSinPure()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItemSinusoidalFuncExposedBP::TransformedCosPure()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

// Called every frame
void AItemSinusoidalFuncExposedBP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	//float DeltaZ = Amplitude * FMath::Sin(RunningTime * TimeConstant);

	//AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));

	DRAW_SPHERE_SINGLE_FRAME(GetActorLocation());
	DRAW_VECTOR_SINGLE_FRAME(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f);

}

