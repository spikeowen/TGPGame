// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponSpawnerChest.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UHealthComponent;
class UWeaponSpawnerComponent;
class AItemActor;

UCLASS()
class TGP_API AWeaponSpawnerChest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponSpawnerChest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = true)) UStaticMeshComponent* _staticMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = true)) USceneComponent* _sceneRoot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = true)) UHealthComponent* _health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = true)) UWeaponSpawnerComponent* _weaponSpawn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventorySettings", Meta = (AllowPrivateAccess = true)) TSubclassOf<AItemActor> _itemActorClass;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION() void OpenChest(AController* causer);
};

