// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/UI/UserWidgetTest.h"

#include "Components/TextBlock.h"

void UUserWidgetTest::SetText(int num)
{
	FText text = FText::AsNumber(num);

	TitleLabel->SetText(text);
}
