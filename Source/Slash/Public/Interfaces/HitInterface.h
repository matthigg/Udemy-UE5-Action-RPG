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
	virtual void GetHit(const FVector& ImpactPoint) = 0;
};
