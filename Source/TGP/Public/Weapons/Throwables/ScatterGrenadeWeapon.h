// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Throwables/GrenadeWeapon.h"
#include "ScatterGrenadeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TGP_API AScatterGrenadeWeapon : public AGrenadeWeapon
{
	GENERATED_BODY()
	protected:
	virtual void ExplodeGrenade() override;
	void SpawnOtherNades();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpawnedNades", Meta = (AllowPrivateAccess = true)) TSubclassOf<class AGrenadeWeapon> _nadeToSpawn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpawnedNades", Meta = (AllowPrivateAccess = true)) UThrowableInfo* _nadeToSpawnInfo;
};
