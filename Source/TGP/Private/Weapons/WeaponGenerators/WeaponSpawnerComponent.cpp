// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/WeaponGenerators/WeaponSpawnerComponent.h"

#include "Kismet/GameplayStatics.h"
#include "TGP/TGPGameModeBase.h"
#include "Item/ItemActor.h"
#include "Item/ItemInfo.h"
#include "GameInstance/BaseGameInstance.h"

// Sets default values for this component's properties
UWeaponSpawnerComponent::UWeaponSpawnerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	_itemSpawnMode = EItemSpawn::Random;
	_spawnAmount = 1;
}


void UWeaponSpawnerComponent::SpawnWeapon()
{
	if (GetOwner()->HasAuthority())
	{
		for(int i = 0; i < _spawnAmount; i++)
		{
			switch(_itemSpawnMode)
			{
			case EItemSpawn::Random:
				SpawnRandomWeapon();
				break;
			case EItemSpawn::Set:
				SpawnSetWeapon();
				break;
			case EItemSpawn::RandomRarity:
				SpawnRandomRarityWeapon();
				break;
			default:
				SpawnRandomWeapon();
				break;
			}	
		}
	}
}

void UWeaponSpawnerComponent::BeginPlay()
{
}

void UWeaponSpawnerComponent::SpawnRandomWeapon()
{
	bool validItem = false;
	UBaseGameInstance* instance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ATGPGameModeBase* GameMode = Cast<ATGPGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	UGunItem* newGun = nullptr;
	while(!validItem)
	{
		newGun = GameMode->CreateItemByUniqueId<UGunItem>(instance->GetRandomItemIdOfCategory(EItemCategory::Gun), 1);
		UItemInfo* info = newGun->GetItemInfo();
		if(info->WeaponType != EWeaponType::Sword)
			validItem = true;
	}
	if(newGun)
	{
		AItemActor* ItemActor = GetWorld()->SpawnActor<AItemActor>(_itemActorClass, GetOwner()->GetActorLocation(), FRotator());
		ItemActor->Initialize(newGun);
		ItemActor->AddInitialThrowForce(FVector(0.0f, 0.0f, 1.0f), 400000.0f);
	}
}

void UWeaponSpawnerComponent::SpawnRandomRarityWeapon()
{
	bool validItem = false;
	UBaseGameInstance* instance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ATGPGameModeBase* GameMode = Cast<ATGPGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	UGunItem* newGun = nullptr;
	while(!validItem)
	{
		newGun = GameMode->CreateItemByUniqueId<UGunItem>(instance->GetRandomItemIdOfCategory(EItemCategory::Gun, _itemRarity), 1);
		UItemInfo* info = newGun->GetItemInfo();
		if(info->WeaponType != EWeaponType::Sword)
			validItem = true;
	}
	if(newGun)
	{
		AItemActor* ItemActor = GetWorld()->SpawnActor<AItemActor>(_itemActorClass, GetOwner()->GetActorLocation(), FRotator());
		ItemActor->Initialize(newGun);
		ItemActor->AddInitialThrowForce(FVector(0.0f, 0.0f, 1.0f), 400000.0f);
	}
}

void UWeaponSpawnerComponent::SpawnSetWeapon()
{
	if(_gunInfo)
	{
		ATGPGameModeBase* GameMode = Cast<ATGPGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		UGunItem* newGun = GameMode->CreateItemByUniqueId<UGunItem>(_gunInfo->UniqueId, 1);
		AItemActor* ItemActor = GetWorld()->SpawnActor<AItemActor>(_itemActorClass, GetOwner()->GetActorLocation(), FRotator());
		ItemActor->Initialize(newGun);
		ItemActor->AddInitialThrowForce(FVector(0.0f, 0.0f, 1.0f), 400000.0f);
	}
}


