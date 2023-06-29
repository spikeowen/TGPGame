#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemContainer.generated.h"

class UBaseItem;

UCLASS()
class TGP_API UItemContainer : public UObject
{
	GENERATED_BODY()

	UPROPERTY(Replicated) TArray<UBaseItem*> Items;
	
public:
	UItemContainer();

	void Initialize(int ContainerSize);

	UBaseItem* GetItemAt(int Slot);
	int FindFirstValidItem();
	int FindFirstInValidItem();
	
	bool AddItem(UBaseItem* Item);
	bool AddItem(UBaseItem* Item, int Slot);
	
	bool RemoveItem(UBaseItem* Item);
	bool RemoveItem(int Index, int Amount);
	bool RemoveItem(int Slot);

	bool InBounds(int Slot) const;

	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	bool ReplicateItems(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags);
	
	UPROPERTY(Replicated) int DebugId;
};
