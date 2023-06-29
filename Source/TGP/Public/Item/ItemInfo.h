#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemInfo.generated.h"
UENUM(BlueprintType)
enum EWeaponType
{
	TwoHand		UMETA(DisplayName = "Rifles and Shotgun"),
	OneHand		UMETA(DisplayName = "Pistol"),
	Sword		UMETA(DisplayName = "Melee Weapon")
};
UENUM()
enum EItemCategory
{
	Default			UMETA(DisplayName = "Default"),
	Weapon			UMETA(DisplayName = "Weapon"),
	Gun				UMETA(DisplayName = "Gun"),
	Consumable		UMETA(DisplayName = "Consumable"),
	Attachment		UMETA(DisplayName = "Attachment")
};

UENUM()
enum ERarity
{
	Common		UMETA(DisplayName = "Common"),
	Uncommon    UMETA(DisplayName = "Uncommon"),
	Rare		UMETA(DisplayName = "Rare"),
	SuperRare   UMETA(DisplayName = "SuperRare"),
	Legendary   UMETA(DisplayName = "Legendary")
};

UENUM()
enum EFireType
{
	Single		UMETA(DisplayName = "Single"),
	Burst		UMETA(DisplayName = "Burst"),
	Auto		UMETA(DisplayName = "Auto")
};

UCLASS(BlueprintType)
class TGP_API UItemInfo : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UItemInfo();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemInfo") TEnumAsByte<EItemCategory> ItemCategory;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemInfo") FString ItemName = "Undefined";
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemInfo") FString ItemDescription = "Undefined";
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemInfo") UStaticMesh* ItemMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData") TEnumAsByte<ERarity> ItemRarity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData") TEnumAsByte<EWeaponType> WeaponType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData") int MaxStack;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData") int TradePrice;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData") UTexture2D* ItemIcon;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, BlueprintReadOnly, Category = "ItemData") TSubclassOf<class UBaseItem> ItemClass; // TODO: REPLACE WITH ITEM CLASS

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Identifiers") FString ItemShortName;
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Identifiers") int32 UniqueId;

	UFUNCTION(CallInEditor, Category = "Identifiers") virtual void GenerateStats();
	UFUNCTION(CallInEditor, Category = "Identifiers") virtual void GenerateNewId();
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};

UCLASS()
class TGP_API UWeaponInfo : public UItemInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo") float Damage = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponVisuals") USkeletalMesh* WeaponSkeletalMesh;

	virtual bool IsSupportedForNetworking() const override { return true; }
};

UCLASS()
class TGP_API UGunInfo : public UWeaponInfo
{
	GENERATED_BODY()

public:
	UGunInfo();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") float ReloadSpeed = 3.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") float AttackRate = 0.03f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") float Recoil = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") int ClipSize = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") int DefaultAmmoReserve = 120;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") TEnumAsByte<EFireType> FireType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") FVector2D Spread = FVector2D(1.f, 1.f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") int BulletsPerShot = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") UCurveFloat* RecoilCurvePitch;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") UCurveFloat* RecoilCurveYaw;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") float RecoilRecoveryModifier = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") TSubclassOf<class UWeaponComponent> BaseWeaponClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") class UThrowableInfo* ProjectileToSpawn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") float ProjectileLaunchSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") bool RequiresTracer = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") float ADSMultiplier = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunInfo") bool UnlimitedAmmo = false;

	virtual void GenerateStats() override;
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};

UCLASS()
class TGP_API UThrowableInfo : public UWeaponInfo
{
	GENERATED_BODY()

public:
	UThrowableInfo();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowableInfo") float ExplosionRadius = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowableInfo") float KnockbackForce = 1000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowableInfo") float FuseTime = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowableInfo") float ThrowForce = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowableInfo") TSubclassOf<AActor> ThrowableBlueprint;

	virtual void GenerateStats() override;
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};