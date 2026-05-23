// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSinusoidalVarsExposedDetails.generated.h"

UCLASS()
class SLASH_API AItemSinusoidalVarsExposedDetails : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemSinusoidalVarsExposedDetails();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	// Visible in details panel, but only for the class itself (not for instances of the class), and not editable in the editor
	//UPROPERTY(VisibleDefaultsOnly); 
	// Visible in details panel, but only for instances of the class (not for the class itself), and not editable in the editor
	UPROPERTY(VisibleInstanceOnly);
	float RunningTime;

	// Visible & editable in details panel,  but only for the class itself (not for instances of the class)
	UPROPERTY(EditDefaultsOnly);
	float Amplitude = 0.25f;

	// Visible & editable in details panel, but only for instances of the class (not for the class itself)
	UPROPERTY(EditInstanceOnly);
	float TimeConstant = 5.f;

	// Note: VisibleAnywhere and EditAnywhere are also available and allow visibility and editability for both the class and 
	// instances of the class

};
