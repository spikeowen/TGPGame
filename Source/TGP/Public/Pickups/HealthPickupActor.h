// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/PickupActor.h"
#include "HealthPickupActor.generated.h"

/**
 * 
 */
UCLASS()
class TGP_API AHealthPickupActor : public APickupActor
{
	GENERATED_BODY()
	protected:
	virtual void OnOverlapWithActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess = true)) int _healthIncrease;
};
