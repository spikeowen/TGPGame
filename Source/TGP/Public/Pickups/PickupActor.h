// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.generated.h"

class UStaticMeshComponent;
class USceneComponent;

UENUM()
enum PickupType
{
	Ammo			UMETA(DisplayName = "Ammo"),
	Health			UMETA(DisplayName = "Health")
};

UCLASS()
class TGP_API APickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION() virtual void OnOverlapWithActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = true)) USceneComponent* _root;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = true)) UStaticMeshComponent* _staticMesh;

	PickupType _itemPickupType;


	// Visuals
	float _t;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	PickupType GetPickupType() { return _itemPickupType; }
};
