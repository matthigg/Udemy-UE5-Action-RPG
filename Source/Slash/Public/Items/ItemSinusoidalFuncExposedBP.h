// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSinusoidalFuncExposedBP.generated.h"

UCLASS()
class SLASH_API AItemSinusoidalFuncExposedBP : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemSinusoidalFuncExposedBP();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters");
	float Amplitude = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters");
	float TimeConstant = 5.f;

	// BlueprintCallable means that this function can be called from Blueprints, and it will also be visible in the editor when you
	// select an instance of AItem.
	UFUNCTION(BlueprintCallable)
	float TransformedSin(float Value);

	// BlueprintPure means that this function does not modify the state of the object and can be used in Blueprint expressions 
	// without needing to be called as a separate node.
	UFUNCTION(BlueprintPure)
	float TransformedSinPure();

	UFUNCTION(BlueprintPure)
	float TransformedCosPure();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	float RunningTime;

};
