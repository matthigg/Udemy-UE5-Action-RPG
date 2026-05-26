// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class SLASH_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:

	// Note: This variable has to have the name as the one it's bound to in a blueprint widget
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;
	
};
