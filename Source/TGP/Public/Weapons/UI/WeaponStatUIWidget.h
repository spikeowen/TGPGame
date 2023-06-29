// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponStatUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class TGP_API UWeaponStatUIWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* StatLabel;

	void SetText(const FString text);
};
