// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"
// We're including the header file for UCapsuleComponent here in the .cpp file, which is where we actually create
// an instance of it and call its methods. This allows us to avoid including the full header file in the .h file,
// which can help reduce compile times and dependencies between files.
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateOptionalDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetCapsuleHalfHeight(20.f);
	Capsule->SetCapsuleRadius(15.f);

	// Set the capsule as the root component of the actor either through assignment or by calling SetRootComponent. 
	// The former is more efficient, but the latter is more flexible and allows you to set the root component to 
	// something else later on.
	//RootComponent = Capsule;
	SetRootComponent(Capsule);

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));

	// This is the more flexible way to set up the attachment, but it can be less efficient than directly assigning the root component.
	//BirdMesh->SetupAttachment(RootComponent
	
	// This is the same as the previous line, but it calls GetRootComponent() to get the root component instead of directly using 
	// RootComponent. This can be useful if you want to change the root component later on.
	BirdMesh->SetupAttachment(GetRootComponent());	

	// This is the most efficient way to set up the attachment, but it can be less flexible if you want to change the root component 
	// later on.
	//BirdMesh->SetupAttachment(Capsule);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;

	// Possess the pawn with the first player controller, which allows us to receive input from the player. This is a simple way to
	// set up input for a pawn, but in a real project, you might want to use a more robust system for handling input and possession.
	AutoPossessPlayer = EAutoReceiveInput::Player0;

}

void ABird::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABird::MoveForward(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("MoveForward called with value: %f"), Value);

	//if (Controller && (Value != 0.f)) // You can also use Controller != nullptr instead of just Controller, which is a more 
	// explicit way to check if the pointer is valid.
	if ((Controller != nullptr) && (Value != 0.f))
	{
		// This is a function that returns the forward vector of the actor, which is a unit vector that points in the direction
		// the actor is facing. 
		//
		// If the Value parameter is positive, the actor will move forward in the direction it's facing. 
		// If the Value parameter is negative, the actor will move backward in the opposite direction. 
		// If the Value parameter is zero, the actor will not move at all.
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, Value);
	}
}

void ABird::Turn(float Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Turn called with value: %f"), Value);
	AddControllerYawInput(Value);
}

// Not working for some reason
void ABird::LookUp(float Value)
{
	UE_LOG(LogTemp, Warning, TEXT("LookUp called with value: %f"), Value);
	AddControllerPitchInput(Value);
}

// Not working for some reason
void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// In this example, we're binding the "MoveForward" axis to the MoveForward function in our ABird class. 
	// This means that when the player presses the input associated with the "MoveForward" axis (e.g. W key
	// or up arrow), the MoveForward function will be called with a value that represents how much the input
	// is being pressed (e.g. 1 for fully pressed, 0 for not pressed).
	//Note: Axis is deprecated: https://www.youtube.com/watch?v=bIo97TLsXkY
	//PlayerInputComponent->BindAxis(TEXT("MoveForward"));
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ABird::MoveForward);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ABird::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ABird::LookUp);



}

