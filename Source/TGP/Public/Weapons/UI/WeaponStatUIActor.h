// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/ItemInfo.h"
#include "WeaponStatUIActor.generated.h"

class USceneComponent;
class UWidgetComponent;
class UWeaponInfo;

UCLASS()
class TGP_API AWeaponStatUIActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponStatUIActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY() class USceneComponent* _root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) class UWidgetComponent* _widget;

	UPROPERTY() APlayerController* _playerController;
		
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void SetWeaponStats(UWeaponInfo* info);
	
	UWidgetComponent* GetWidget() { return _widget; }
};
