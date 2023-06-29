// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TGP/FP_FirstPerson/FP_FirstPersonCharacter.h"
#include "WeaponComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFireSuccess, FVector, dir, float, damage);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TGP_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool CheckMouseReleased();

	FVector BulletSpreadCalculation(FVector dir, FVector up, FVector right, FVector2D spread);
	
	UPROPERTY() AActor* _parent;
	UPROPERTY() AFP_FirstPersonCharacter* _player;
	UPROPERTY() APlayerController* _parentController;
	UPROPERTY() USceneComponent* _holdPoint;

	UPROPERTY() class USkeletalMeshComponent* _parentMesh;
	
	bool _canUse;
	bool _singleFireCheck;

	bool _IsADS;
	
	UPROPERTY() class UGunItem* _weaponItem;
	UPROPERTY() class UGunInfo* _weaponInfo;
	
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual bool OnFire();
	virtual void OnFireEnd();
	
	void PickupWeapon(AActor* parent);
	virtual void DropWeapon();
	void SetParentMesh(class USkeletalMeshComponent* mesh) { _parentMesh = mesh; }
	virtual void InitializeWeapon(class UGunItem* gunItem);
	UFUNCTION(BlueprintCallable) UGunInfo* GetWeaponInfo() { return _weaponInfo; }
	UFUNCTION(BlueprintCallable) virtual FVector2D GetCurrentAmmo() { return FVector2D(); };
	UPROPERTY(BlueprintAssignable, Category = "Components") FOnFireSuccess onFireSuccess;

	void SetADS(bool value) { _IsADS = value;}
	
	void StartUse(){ _canUse = false;	}
	void EndUse(){ _canUse = true;	}
};
