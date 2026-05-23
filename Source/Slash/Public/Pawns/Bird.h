// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
// Note: it's not a good idea to include header files in header files, but for the sake of this example, we'll do it here. 
// In a real project, you might want to forward declare UCapsuleComponent and include the header in the .cpp file instead.
// 
// Forward declaration is declaring a class type without actually defining it, e.g. class UCapsuleComponent* Capsule; This 
// allows you to use pointers or references to the class without needing the full definition, which can help reduce compile
// times and dependencies between files.
//
// When forward declaring a class, that class is considered an "Incomplete Type". This means that it's a type that describes
// an identifier without information needed to determine its size. As a result, you can only declare pointers or references
// to the class, and you cannot create instances of it or access its members.
//
// In the corresponding *.cpp file, you would include the header file for UCapsuleComponent to get the full definition and
// be able to use it properly. In this case, we would include it in Bird.cpp, which is where we actually create an instance
// of UCapsuleComponent and call its methods
//#include "Components/CapsuleComponent.h" 
#include "Bird.generated.h"

// Here we're forward declaring the UCapsuleComponent class at the top of the file.
class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class SLASH_API ABird : public APawn
{
	GENERATED_BODY()

public:
	ABird();
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void Turn(float Value);
	void LookUp(float Value);

private:

	// In order to get this to compile we had to include the header file for UCapsuleComponent, which is not ideal. The
	// reason is that #include "Components/CapsuleComponent.h" will include literally the entire header file for the capsule 
	// component, which can be quite large and can lead to longer compile times. In a real project, you might want to forward 
	// declare UCapsuleComponent and include the header in the .cpp file instead.
	//UCapsuleComponent* Capsule;

	// Forward declaration of UCapsuleComponent, which allows us to declare a pointer to it without including the full header file.
	// Alternatively, you can forward declare the class at the top of the header file.
	//class UCapsuleComponent* Capsule;

	// Once a component class has been forward declared, you can declare additional pointers or references to it, but you cannot create 
	// instances of it or access its members.
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Capsule;

	//UCapsuleComponent* SecondCapsule;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* BirdMesh;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

};
