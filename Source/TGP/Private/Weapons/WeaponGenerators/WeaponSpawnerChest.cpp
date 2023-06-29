// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponGenerators/WeaponSpawnerChest.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/HealthComponent.h"
#include "TGP/TGPGameModeBase.h"
#include "Item/ItemActor.h"
#include "Weapons/WeaponGenerators/WeaponSpawnerComponent.h"

// Sets default values
AWeaponSpawnerChest::AWeaponSpawnerChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_sceneRoot = _staticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	_health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	AddOwnedComponent(_health);

	_weaponSpawn = CreateDefaultSubobject<UWeaponSpawnerComponent>(TEXT("Weapon Spawn Component"));
	AddOwnedComponent(_weaponSpawn);
}

// Called when the game starts or when spawned
void AWeaponSpawnerChest::BeginPlay()
{
	Super::BeginPlay();
	
	_health->onComponentDead.AddDynamic(this, &AWeaponSpawnerChest::OpenChest);
}

// Called every frame
void AWeaponSpawnerChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponSpawnerChest::OpenChest(AController* causer)
{
	_weaponSpawn->SpawnWeapon();
}
