// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponComponent.h"
#include "Components/SceneComponent.h"
#include "Item/BaseItem.h"
#include "Item/ItemInfo.h"
#include "Components/SkeletalMeshComponent.h"
#include "TGP/FP_FirstPerson/FP_FirstPersonCharacter.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	// ...
	_canUse = true;
	_singleFireCheck = false;
	_weaponInfo = nullptr;
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

bool UWeaponComponent::CheckMouseReleased()
{
	if(_weaponInfo->FireType == EFireType::Single)
	{
		if(!_singleFireCheck)
		{
			_singleFireCheck = true;
			return true;
		}
		return false;
	}
	return true;
}

FVector UWeaponComponent::BulletSpreadCalculation(FVector dir, FVector up, FVector right, FVector2D spread)
{
	float deviation = FMath::RandRange(-spread.X, spread.X);
	float angle = FMath::RandRange(-spread.Y, spread.Y);
	dir = dir.RotateAngleAxis(deviation, up);
	dir = dir.RotateAngleAxis(angle, right);
	return dir;
}

bool UWeaponComponent::OnFire()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Pew Pew"));
	return true;
}

void UWeaponComponent::OnFireEnd()
{

}

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponComponent::PickupWeapon(AActor* parent)
{
	if(_parent == nullptr)
	{
		_parent = parent;
		_player = Cast<AFP_FirstPersonCharacter>(_parent);
		_parentController = nullptr;
	}
}

void UWeaponComponent::DropWeapon()
{
	
}

void UWeaponComponent::InitializeWeapon(UGunItem* gunItem) // Get and Update ammo count from _weaponItem
{
	DropWeapon();
	
	_weaponItem = gunItem;
	_weaponInfo = Cast<UGunInfo>(gunItem->GetItemInfo());
}