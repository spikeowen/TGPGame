// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/AmmoPickupActor.h"

#include "TGP/FP_FirstPerson/FP_FirstPersonCharacter.h"
#include "Weapons/WeaponComponent.h"
#include "Weapons/Interfaces/WeaponInterfaces.h"

void AAmmoPickupActor::OnOverlapWithActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->ActorHasTag("Player"))
	{
		AFP_FirstPersonCharacter* character = Cast<AFP_FirstPersonCharacter>(OtherActor);
		if(character)
		{
			UWeaponComponent* comp = character->GetCurrentWeaponComponent();
			IHasAmmo* ammo = Cast<IHasAmmo>(comp);
			if(ammo)
			{
				ammo->currentReserves += _ammoIncrease;
				Destroy();
			}
		}
	}
}
