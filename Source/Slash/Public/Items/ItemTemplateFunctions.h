// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemTemplateFunctions.generated.h"

UCLASS()
class SLASH_API AItemTemplateFunctions : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemTemplateFunctions();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters");
	float Amplitude = 0.95f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters");
	float TimeConstant = 5.f;

	UFUNCTION(BlueprintCallable)
	float TransformedSin(float Value);

	UFUNCTION(BlueprintPure)
	float TransformedSinPure();

	UFUNCTION(BlueprintPure)
	float TransformedCosPure();

	template<typename T>
	T Avg(T First, T Second);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	float RunningTime;

};

template<typename T>
inline T AItemTemplateFunctions::Avg(T First, T Second)
{
	return (First + Second) / 2;
}
