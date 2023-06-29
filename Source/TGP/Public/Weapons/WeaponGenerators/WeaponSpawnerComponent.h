// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "WeaponSpawnerComponent.generated.h"

class AItemActor;
class UGunInfo;

UENUM()
enum EItemSpawn
{
	Set		UMETA(DisplayName = "Set"),
	Random		UMETA(DisplayName = "Random"),
	RandomRarity		UMETA(DisplayName = "RandomRarity")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TGP_API UWeaponSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponSpawnerComponent();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Actor", Meta = (AllowPrivateAccess = true)) TSubclassOf<AItemActor> _itemActorClass;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SpawnRandomWeapon();
	void SpawnRandomRarityWeapon();
	void SpawnSetWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Info", Meta = (AllowPrivateAccess = true)) UGunInfo* _gunInfo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Info", Meta = (AllowPrivateAccess = true)) TEnumAsByte<EItemSpawn> _itemSpawnMode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Info", Meta = (AllowPrivateAccess = true)) TEnumAsByte<enum ERarity> _itemRarity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Info", Meta = (AllowPrivateAccess = true)) int _spawnAmount;
	
public:	
	void SpawnWeapon();
		
};
