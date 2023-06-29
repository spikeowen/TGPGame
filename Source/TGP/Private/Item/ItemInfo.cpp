// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemInfo.h"
#include "Item/BaseItem.h"
#include "Net/UnrealNetwork.h"

void UItemInfo::GenerateStats()
{
	ItemShortName = FString::Printf(TEXT("%s.%s"), *UEnum::GetValueAsString(ItemCategory), *ItemName);
}

void UItemInfo::GenerateNewId()
{
	FMath::RandInit(FDateTime::Now().GetMillisecond() + FDateTime::Now().GetSecond());
	UniqueId = FMath::RandRange(78000, INT32_MAX);
}

FPrimaryAssetId UItemInfo::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("ItemInfo", GetFName());
}

UItemInfo::UItemInfo()
{
	UniqueId = 0;
	ItemClass = UBaseItem::StaticClass();
}

FPrimaryAssetId UGunInfo::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("ItemInfo", GetFName());
}

UGunInfo::UGunInfo()
{
	ItemClass = UGunItem::StaticClass();
	ItemCategory = EItemCategory::Gun;
}

void UGunInfo::GenerateStats()
{
	Super::GenerateStats();

	ItemShortName = FString::Printf(TEXT("%s.%s.%s"), *UEnum::GetValueAsString(ItemCategory), *FString("Gun"), *ItemName);
}

UThrowableInfo::UThrowableInfo()
{
	ItemClass = UThrowableItem::StaticClass();
	ItemCategory = EItemCategory::Weapon;
}

FPrimaryAssetId UThrowableInfo::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("ItemInfo", GetFName());
}

void UThrowableInfo::GenerateStats()
{
	Super::GenerateStats();
	ItemShortName = FString::Printf(TEXT("%s.%s.%s"), *UEnum::GetValueAsString(ItemCategory), *FString("Throwable"), *ItemName);
}
