// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Items/Treasure.h"

#include "PickupInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_API IPickupInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// When using these virtual interface functions you have to set them = 0, e.g.
	// virtual void SetOverlappingItem(class AItem* Item) = 0;, --unless-- you want to provide a default implementation 
	// for the function, in which case you can just leave it as is and implement the function in the .cpp file. 
	// This allows you to avoid having to implement the function in every class that inherits from this interface, 
	// and only implement it in the classes that need it.
	virtual void SetOverlappingItem(class AItem* Item);
	virtual void AddSouls(class ASoul* Soul);
	virtual void AddGold(class ATreasure* Treasure);
};
