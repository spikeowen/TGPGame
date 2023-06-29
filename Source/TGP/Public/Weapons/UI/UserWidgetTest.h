// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidgetTest.generated.h"

/**
 * 
 */
UCLASS()
class TGP_API UUserWidgetTest : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetText(int num);
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TitleLabel;
};
