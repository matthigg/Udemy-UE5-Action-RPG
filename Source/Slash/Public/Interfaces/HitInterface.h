// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_API IHitInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// Pure virtual functions cannot be implemented in the class in which they are declared, instead they are
	// intended to be inherited and then implemented in another class. To do this, first set them to zero in
	// an interface header (such as this one, HitInterface.h), e.g. virtual void SomeFunction() = 0; Next,
	// inherit this pure virtual function in another class via that class' header file. To see an example,
	// this interface is currently inherited in Enemy.h.
	//
	// We're also creating a const parameter that takes a reference/pointer input argument, and this does 2
	// things 1) Prevents changing/modifying the input argument, and 2) Passes a reference/address to the
	// argument instead of the entire object.
	//virtual void GetHit(const FVector& ImpactPoint) = 0;

	// Blueprint native events allow us to have both C++ and blueprint functionality. While GetHit is not
	// marked as "virtual" it is still overridable, but it has to be implemented in a certain way. When
	// implementing this function in a class that inherits this interface, e.g. Enemy.cpp, we have to implement
	// the function with the _Implementation suffix, e.g. void AEnemy::GetHit_Implementation(const FVector& ImpactPoint). 
	// This is because the function is marked as a BlueprintNativeEvent, which means it can be implemented in 
	// both C++ and blueprints. If we were to implement this function without the _Implementation suffix, it would 
	// not be recognized as an override of the interface function, and we would get a compiler error.

	// Additionally we have to prefix calling the function with Execute_ when calling it from blueprints, 
	// e.g. Execute_GetHit(...). This is because the function is marked as a BlueprintNativeEvent, which means 
	// it can be called from both C++ and blueprints. If we were to call this function without the Execute_ prefix, 
	// it would not be recognized as a valid function call in blueprints, and we would get a compiler error.
	UFUNCTION(BlueprintNativeEvent)
	void GetHit(const FVector& ImpactPoint);

};
