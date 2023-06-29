// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/ItemInfo.h"
#include "Projectile.generated.h"

UCLASS()
class TGP_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	FVector _direction;
	float _speed;
	UPROPERTY() APlayerController* _spawnedBy;
	UPROPERTY() UThrowableInfo* _throwableInfo;
	UPROPERTY() USkeletalMeshComponent* _physicsMeshReference;


	void SetPhysicsMesh(USkeletalMeshComponent* mesh) { _physicsMeshReference = mesh; }
public:	
	virtual void SetProjectileParameters(APlayerController* spawnedBy, FVector dir, float speed);
	virtual void Initialize(UThrowableInfo* throwableInfo) { _throwableInfo = throwableInfo; _physicsMeshReference->SetSkeletalMesh(throwableInfo->WeaponSkeletalMesh);}
};
