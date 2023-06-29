// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunHostActor.generated.h"

UCLASS()
class TGP_API AGunHostActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGunHostActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay) class UStaticMeshComponent* _mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay) class USceneComponent* _sceneRoot;

	UPROPERTY(EditAnywhere) class UWeaponComponent* _weapon;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DebugMessage();
	class UWeaponComponent* GetWeaponComponent() { return _weapon; }
	void AssignNewComponent(UWeaponComponent* component);
};
