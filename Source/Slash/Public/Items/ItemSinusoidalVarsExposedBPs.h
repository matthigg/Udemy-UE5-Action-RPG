// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSinusoidalVarsExposedBPs.generated.h"

UCLASS()
class SLASH_API AItemSinusoidalVarsExposedBPs : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemSinusoidalVarsExposedBPs();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Visible & editable in details panel,  but only for the class itself (not for instances of the class)
	// BlueprintReadOnly allows the variable to be read in Blueprints, but not modified. It can be used with
	// EditAnywhere or EditDefaultsOnly, but not with EditInstanceOnly
	//UPROPERTY(EditAnywhere, BlueprintReadOnly);

	// BlueprintReadWrite allows the variable to be read and modified in Blueprints. It can be used with 
	// EditAnywhere, EditDefaultsOnly, or EditInstanceOnly
	// The Category parameter is used to group properties in the editor. Properties with the same category 
	// will be grouped together in the details panel. If no category is specified, the property will be placed
	// in the "Default" category.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters");
	float Amplitude = 0.25f;

	// Visible & editable in details panel, but only for instances of the class (not for the class itself)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters");
	float TimeConstant = 5.f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	// Visible in details panel, but only for the class itself (not for instances of the class), and not editable in the editor
	//UPROPERTY(VisibleDefaultsOnly); 
	// Visible in details panel, but only for instances of the class (not for the class itself), and not editable in the editor
	//UPROPERTY(VisibleInstanceOnly); 

	// Visible in details panel, but only for instances of the class (not for the class itself), and not editable in the editor, 
	// and read-only in Blueprints (the Event Graph)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	float RunningTime;

};
