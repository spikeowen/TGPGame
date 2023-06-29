
#include "Item/BaseItem.h"

#include "Engine/ActorChannel.h"
#include "Item/ItemInfo.h"
#include "Inventory/ItemContainer.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "GameInstance/BaseGameInstance.h"

UBaseItem::UBaseItem()
{
	
}

void UBaseItem::Init(UItemInfo* Info, int Amount)
{
	this->ItemInfo = Info;
	this->ItemAmount = Amount;
	this->ItemId = ItemInfo->UniqueId;
}

void UBaseItem::OnUse()
{
	ItemAmount -= 1;

	if (ItemAmount <= 0)
		OwningContainer->RemoveItem(this);
}

int UBaseItem::TryStack(UBaseItem* Item)
{
	if (Item->ItemInfo->UniqueId == ItemInfo->UniqueId)
	{
		ItemAmount += Item->GetAmount();

		if (ItemAmount >= ItemInfo->MaxStack)
		{
			const int Remaining = ItemAmount - ItemInfo->MaxStack;
			ItemAmount = ItemInfo->MaxStack;

			return Remaining;
		}

		return 0;
	}

	return Item->GetAmount();
}

void UGunItem::Init(UItemInfo* Info, int Amount)
{
	Super::Init(Info, Amount);

	const UGunInfo* GunInfo = Cast<UGunInfo>(Info);
	AmmoInClip = GunInfo->ClipSize;
	AmmoCount = GunInfo->DefaultAmmoReserve;
}

void UBaseItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	//UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UBaseItem, ItemId);
	DOREPLIFETIME(UBaseItem, ItemAmount);
}

void UBaseItem::OnRep_UpdateItemInfo()
{
	UpdateItemInfo();
}

void UBaseItem::UpdateItemInfo()
{
	if (ItemInfo == nullptr || ItemInfo->UniqueId != ItemId)
		ItemInfo = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->FindInfoUniqueId(ItemId);
}

void UGunItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGunItem, AmmoInClip);
	DOREPLIFETIME(UGunItem, AmmoCount);
}
