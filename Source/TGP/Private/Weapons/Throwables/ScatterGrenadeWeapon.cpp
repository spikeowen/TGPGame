// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Throwables/ScatterGrenadeWeapon.h"

#include "Kismet/GameplayStatics.h"
#include "Weapons/Throwables/GrenadeWeapon.h"
#include "Sound/SoundCue.h"

void AScatterGrenadeWeapon::ExplodeGrenade()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BOOM"));
	UGameplayStatics::PlaySound2D(this, NadeBoom);
	_particleSystem->Activate();
	ItemSkeletalMesh->SetHiddenInGame(true);

	SphereCastForTargets();
	
	SpawnOtherNades();

	FTimerHandle explosionTimeHandler;
	GetWorld()->GetTimerManager().SetTimer(explosionTimeHandler, this, &AGrenadeWeapon::DestroyObj, 1.0f, false);
}

void AScatterGrenadeWeapon::SpawnOtherNades()
{
	if(_nadeToSpawn && _nadeToSpawnInfo)
	{
		TArray<FVector> spawnDirs = {FVector(1.0f, 1.0f, 0.0f), FVector(-1.0f, 1.0f, 0.0f), FVector(-1.0f, -1.0f, 0.0f), FVector(1.0f, -1.0f, 0.0f)};
		for(auto vec : spawnDirs)
		{
			AGrenadeWeapon* ThrowableActor = GetWorld()->SpawnActor<AGrenadeWeapon>(_nadeToSpawn, ItemSkeletalMesh->GetComponentLocation(), FRotator());
			ThrowableActor->Initialize(_nadeToSpawnInfo);
			ThrowableActor->SetProjectileParameters(_spawnedBy, vec, 10000.0f);
		}
	}
}
