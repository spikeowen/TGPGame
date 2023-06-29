#pragma once

#include "CoreMinimal.h"
#include "ItemContainer.h"
#include "Components/ActorComponent.h"
#include "Item/ItemActor.h"
#include "PlayerInventory.generated.h"

class UItemContainer;
class UWeaponItem;
class UThrowableItem;
class UBaseItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, UWeaponItem*, WeaponItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAddWeapon);

UENUM()
enum EWeaponSlot
{
	Primary = 0,
	Secondary = 1,
	Melee = 2
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TGP_API UPlayerInventory : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventorySettings", Meta = (AllowPrivateAccess = true)) uint8 MaxUtilityAmount = 4;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventorySettings", Meta = (AllowPrivateAccess = true)) uint8 MaxConsumableAmount = 4;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventorySettings", Meta = (AllowPrivateAccess = true)) TSubclassOf<AItemActor> ItemActorClass;
	
	UPROPERTY(Replicated) UItemContainer* UtilityContainer;
	UPROPERTY(Replicated) UItemContainer* ConsumableContainer;
	UPROPERTY(Replicated) UItemContainer* WeaponContainer;
	
	EWeaponSlot SelectedWeapon = EWeaponSlot::Melee;
	uint8 SelectedUtilitySlot = 0;

	void TryFindAndSelectValidUtility();

	void InitDefaultGuns();

	// Replication \\

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
protected:
	virtual void BeginPlay() override;

public:
	UPlayerInventory();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(NetMulticast, Reliable) void AddWeapon(UWeaponItem* Item, int Slot);
	void AddWeapon_Implementation(UWeaponItem* Item, int Slot);

	UFUNCTION(NetMulticast, Reliable) void AddUtility(UThrowableItem* Item, int Slot);
	//void AddWeapon_Implementation(UWeaponItem* Item, int Slot);
	
	//bool AddUtility(UThrowableItem* ThrowableItem) const;
	void SelectUtility(uint8 Slot);
	UThrowableItem* GetSelectedUtility();

	EWeaponSlot GetSelectedWeaponSlot() const { return SelectedWeapon; }
	TSubclassOf<AItemActor> GetItemActor() const { return ItemActorClass; }

	void ComponentLoadComplete();

	UPROPERTY(BlueprintAssignable, Category="WeaponChangedEvent")
	FOnWeaponChanged OnWeaponChangedEvent;

	UPROPERTY(BlueprintAssignable, Category="WeaponChangedEvent")
	FOnAddWeapon OnTest;
	
	bool PickUpWeapon(UWeaponItem* WeaponItem);
	UFUNCTION(BlueprintCallable) void DropWeapon(int Slot = -1);
	
	UFUNCTION(BlueprintCallable) void ChangeWeapon(EWeaponSlot Slot, bool bForceUpdate = false, bool bBroadcastChange = true);
	UFUNCTION(BlueprintCallable) bool TryPickUpItem(UBaseItem* Item, int SelectedSlot);
	UFUNCTION(BlueprintCallable, BlueprintPure) class UWeaponItem* GetSelectedWeapon();
	
	UFUNCTION(BlueprintCallable) UBaseItem* GetItem() { return WeaponContainer->GetItemAt(0); };
	UFUNCTION(BlueprintCallable) int GetContainerId() { return WeaponContainer->DebugId; };

	UFUNCTION(Server, Reliable, BlueprintCallable) void GenItems();
	void GenItems_Implementation();

	UFUNCTION(BlueprintCallable) void PrintWeaponItems();

	UFUNCTION(Server, Reliable) void SrvDropWeapon(int Slot);
	void SrvDropWeapon_Implementation(int Slot);

	UFUNCTION(BlueprintCallable) int GetNadeAmount() const;
	UFUNCTION(BlueprintCallable) int GetFlashAmount() const;

	UWeaponItem* GetWeaponAtSlot(EWeaponSlot slot);	
	
	void OnUseUtility();
};
