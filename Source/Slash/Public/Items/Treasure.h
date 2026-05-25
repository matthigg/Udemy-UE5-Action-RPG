// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ATreasure : public AItem
{
	GENERATED_BODY()
	
protected:

	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		// Here we're passing in SweepResult by reference to avoid having to make another copy of it, which happens automatically
		// when passing objects into functions, but at the same time we want to avoid modifying any values. Setting the input
		// parameter to const is a trick you can use to avoid accidentally modifying the incoming referenced argument/object.
		const FHitResult& SweepResult
	) override;

private:

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* PickupSound;
};
