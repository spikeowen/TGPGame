// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Throwables/ThrowableWeapon.h"
#include "Item/ItemInfo.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AThrowableWeapon::AThrowableWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AThrowableWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AThrowableWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AThrowableWeapon::SetInitialThrowForce(FVector forceDir)
{
	_physicsMeshReference->AddForce(forceDir);
	FVector randSpin = FVector(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f));
	_physicsMeshReference->AddTorque(randSpin * 50000.0f);
}

void AThrowableWeapon::Initialize(UThrowableInfo* throwableInfo)
{
	_throwableInfo = throwableInfo;
	_physicsMeshReference->SetSkeletalMesh(_throwableInfo->WeaponSkeletalMesh);
}

void AThrowableWeapon::SetPlayerController(APlayerController* controller)
{
	_controller = controller;
}