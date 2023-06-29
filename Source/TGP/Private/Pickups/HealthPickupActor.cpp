// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/HealthPickupActor.h"

#include "TGP/FP_FirstPerson/FP_FirstPersonCharacter.h"
#include "Weapons/Interfaces/WeaponInterfaces.h"
#include "Kismet/GameplayStatics.h"

void AHealthPickupActor::OnOverlapWithActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->ActorHasTag("Player"))
	{
		UGameplayStatics::ApplyDamage(OtherActor, -_healthIncrease, nullptr, this, UDamageType::StaticClass());
		Destroy();
	}
}
