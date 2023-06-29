
#include "Inventory/ItemContainer.h"

#include "Engine/ActorChannel.h"
#include "Item/BaseItem.h"
#include "Net/UnrealNetwork.h"

UItemContainer::UItemContainer()
{
	
}

void UItemContainer::Initialize(int ContainerSize)
{
	Items.Init(nullptr, ContainerSize); // Reserve with NULL items
}

UBaseItem* UItemContainer::GetItemAt(int Slot)
{
	return InBounds(Slot) ? Items[Slot] : nullptr;
}

int UItemContainer::FindFirstValidItem()
{
	for (uint8 i = 0; i < Items.Num(); i++)
		if(Items[i])
			return i;
	
	return -1;
}

int UItemContainer::FindFirstInValidItem()
{
	for (uint8 i = 0; i < Items.Num(); i++)
		if(Items[i] == nullptr)
			return i;
	
	return -1;
}

bool UItemContainer::AddItem(UBaseItem* Item)
{
	if (Item)
	{
		int FirstNullPos = -1;

		for (int i = 0; i < Items.Num(); i++)
		{
			UBaseItem* ItemAtSlot = Items[i];

			if (ItemAtSlot == nullptr && FirstNullPos == -1)
			{
				FirstNullPos = i;
			}
			else if (ItemAtSlot != nullptr)
			{
				Item->SetAmount(ItemAtSlot->TryStack(Item));

				if (Item->GetAmount() <= 0)
					return true;
			}
		}

		if (FirstNullPos != -1)
		{
			Items[FirstNullPos] = Item;
			Item->SetOwningContainer(this);
			return true;
		}
	}

	return false;
}

bool UItemContainer::AddItem(UBaseItem* Item, int Slot)
{
	if (GetItemAt(Slot) == nullptr)
	{
		Items[Slot] = Item;
		return true;
	}

	return false;
}

bool UItemContainer::RemoveItem(UBaseItem* Item)
{
	const int SlotIndex = Items.Find(Item);

	if (SlotIndex != INDEX_NONE)
	{
		RemoveItem(SlotIndex);
		return true;
	}

	return false;
}

bool UItemContainer::RemoveItem(int Index, int Amount)
{
	Items[Index]->SetAmount(Items[Index]->GetAmount() - Amount);

	if (Items[Index]->GetAmount() <= 0)
	{
		Items[Index] = nullptr;
		return true;
	}
	
	return false;
}

bool UItemContainer::RemoveItem(int Slot)
{
	UBaseItem* Item = GetItemAt(Slot);
	
	if (Item)
	{
		Items[Slot] = nullptr;
		return true;
	}

	return false;
}

bool UItemContainer::InBounds(int Slot) const
{
	return Slot >= 0 && Slot < Items.Num();
}

void UItemContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	//UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UItemContainer, Items);
	DOREPLIFETIME(UItemContainer, DebugId);
}

bool UItemContainer::ReplicateItems(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool Replicated = false;

	for	(int i = 0; i < Items.Num(); i++)
		Replicated |= Channel->ReplicateSubobject(Items[i], *Bunch, *RepFlags);
	
	return Replicated;
}