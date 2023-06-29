// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowableWeapon.generated.h"

class UThrowableInfo;
class USkeletalMesh;

UCLASS()
class TGP_API AThrowableWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThrowableWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SetPhysicsMesh(USkeletalMeshComponent* mesh) { _physicsMeshReference = mesh; }
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = true)) UThrowableInfo* _throwableInfo;
	UPROPERTY() APlayerController* _controller;
	UPROPERTY() USkeletalMeshComponent* _physicsMeshReference;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetInitialThrowForce(FVector forceDir);
	virtual void Initialize(UThrowableInfo* throwableInfo);
	virtual void SetPlayerController(APlayerController* controller);

};
