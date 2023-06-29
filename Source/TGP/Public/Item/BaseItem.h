#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseItem.generated.h"

class UItemInfo;
class UItemContainer;

USTRUCT()
struct FItemData
{
	GENERATED_BODY()

public:
	int ItemAmount = 1;
};

USTRUCT()
struct FWeaponData : public FItemData
{
	GENERATED_BODY()

public:
	
};

USTRUCT()
struct FGunData : public FWeaponData
{
	GENERATED_BODY()

public:
	int AmmoCount = 120;
	int AmmoInClip;
	
};

UCLASS(BlueprintType)
class TGP_API UBaseItem : public UObject
{
	GENERATED_BODY()

	UFUNCTION() void OnRep_UpdateItemInfo();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_UpdateItemInfo, BlueprintReadOnly) int ItemId;
	UPROPERTY(Replicated, BlueprintReadOnly) int ItemAmount = 1;
	
	UPROPERTY(BlueprintReadOnly, Meta = (ExposeOnSpawn)) UItemInfo* ItemInfo;
	UPROPERTY() UItemContainer* OwningContainer;

	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UBaseItem();

	UFUNCTION(BlueprintCallable, BlueprintPure) UItemInfo* GetItemInfo() const { return ItemInfo; }
	UFUNCTION(BlueprintCallable, BlueprintPure) int GetItemId() const { return ItemId; }

	virtual void Init(UItemInfo* Info, int Amount);
	void SetOwningContainer(UItemContainer* Container) { OwningContainer = Container; }
	
	int GetAmount() const { return ItemAmount; }
	void SetAmount(int Amount) { ItemAmount = Amount; }

	void OnUse();
	int TryStack(UBaseItem* Item);

	void UpdateItemInfo();
};

UCLASS()
class TGP_API UWeaponItem : public UBaseItem
{
	GENERATED_BODY()

public:
};

UCLASS()
class TGP_API UGunItem : public UWeaponItem
{
	GENERATED_BODY()

	UPROPERTY(Replicated) int AmmoCount = 120;
	UPROPERTY(Replicated) int AmmoInClip;
	
public:
	virtual void Init(UItemInfo* Info, int Amount) override;
	
	UFUNCTION(BlueprintCallable) int GetAmmoInClip() const { return AmmoInClip; }
	void SetAmmoInClip(int NewAmmo) { AmmoInClip = NewAmmo; }
	
	UFUNCTION(BlueprintCallable) int GetAmmoCount() const { return AmmoCount; }
	void SetAmmoCount(int NewAmmoCount) { AmmoCount = NewAmmoCount; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};


UCLASS()
class TGP_API UThrowableItem : public UWeaponItem
{
	GENERATED_BODY()

public:
};