// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/PickupActor.h"
#include "AmmoPickupActor.generated.h"

/**
 * 
 */
UCLASS()
class TGP_API AAmmoPickupActor : public APickupActor
{
	GENERATED_BODY()
	protected:
	virtual void OnOverlapWithActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", Meta = (AllowPrivateAccess = true)) int _ammoIncrease;
};
