// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemTemplateFunctions.h"
#include "Slash/DebugMacros.h"

// Sets default values
AItemTemplateFunctions::AItemTemplateFunctions()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItemTemplateFunctions::BeginPlay()
{
	Super::BeginPlay();

	int32 AvgInt = Avg<int32>(3, 5);
	UE_LOG(LogTemp, Warning, TEXT("Avg of 3 and 5 is %d"), AvgInt);

	float AvgFloat = Avg<float>(3.f, 5.f);
	UE_LOG(LogTemp, Warning, TEXT("Avg of 3.f and 5.f is %f"), AvgFloat);

}

float AItemTemplateFunctions::TransformedSin(float Value)
{
	return Amplitude * FMath::Sin(Value * TimeConstant);
}

float AItemTemplateFunctions::TransformedSinPure()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItemTemplateFunctions::TransformedCosPure()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

// Called every frame
void AItemTemplateFunctions::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	//float DeltaZ = Amplitude * FMath::Sin(RunningTime * TimeConstant);

	//AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));

	DRAW_SPHERE_SINGLE_FRAME(GetActorLocation());
	DRAW_VECTOR_SINGLE_FRAME(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f);

	FVector AvgVector = Avg(GetActorLocation(), FVector::ZeroVector);
	DRAW_POINT_SINGLE_FRAME(AvgVector);

}

