// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemPrintLogsDebugging.h"
#include "Slash/DebugMacros.h"

// Sets default values
AItemPrintLogsDebugging::AItemPrintLogsDebugging()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItemPrintLogsDebugging::BeginPlay()
{
	Super::BeginPlay();

	// Prints to Output Log
	UE_LOG(LogTemp, Warning, TEXT("========== Hello from Item! =========="));

	if (GEngine)
	{
		// Prints to PIE
		GEngine->AddOnScreenDebugMessage(2, 60.f, FColor::Yellow, TEXT("========== GEngine =========="));
	}

	UWorld* World = GetWorld();
	FVector Location = GetActorLocation();
	FVector Forward = GetActorForwardVector();

	DRAW_SPHERE(Location);
	//DRAW_LINE(Location, Location + Forward * 100.f);
	//DRAW_POINT(Location + Forward * 100.f);
	DRAW_VECTOR(Location, Location + Forward * 100.f);

	//if (World)
	//{
	//	// Debug Sphere
	//	DrawDebugSphere(World, Location, 25.f, 24, FColor::Red, false, 30.f);
	//}

	//if (World)
	//{
	//	// Debug Line
	//	DrawDebugLine(World, Location, Location + Forward  * 100.f, FColor::Red, true, -1.f, 0, 1.f);
	//}

	//if (World)
	//{
	//	DrawDebugPoint(World, Location + Forward * 100.f, 25.f, FColor::Yellow, true);
	//}
}

// Called every frame
void AItemPrintLogsDebugging::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Prints to Output Log
	//UE_LOG(LogTemp, Warning, TEXT("========== DeltaTime: %f =========="), DeltaTime);

	if (GEngine)
	{
		FString Name = GetName();
		// We're not using *Name to dereference Name, we're using it in order to use C-style characters,
		// which is an array of characters. Essentially the asterisk is overloaded to convert FString to 
		// const TCHAR* (which is a C-style string) when we use it in this context.
		// From documentation: When using %s parameters to include FStrings, the * operator must be used
		// to return the TCHAR* required for the %s parameter.
		FString Message1 = FString::Printf(TEXT("========== GetName: %s =========="), *Name);

		// Prints to PIE
		GEngine->AddOnScreenDebugMessage(3, 60.f, FColor::Yellow, Message1);

		// Prints to Output Log
		//UE_LOG(LogTemp, Warning, TEXT("========== GetName: %s =========="), *Name);


		if (false)
		{
			FString Message2 = FString::Printf(TEXT("========== DeltaTime: %f =========="), DeltaTime);

			// Prints to PIE
			GEngine->AddOnScreenDebugMessage(4, 60.f, FColor::Yellow, Message2);
		}
	}

}

