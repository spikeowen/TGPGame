// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectiles/Projectile.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AProjectile::SetProjectileParameters(APlayerController* spawnedBy, FVector dir, float speed)
{
	_direction = dir;
	_speed = speed;
	_spawnedBy = spawnedBy;
}


