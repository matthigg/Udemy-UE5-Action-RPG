// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemComponents.h"

// Sets default values
AItemComponents::AItemComponents()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItemComponents::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemComponents::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

