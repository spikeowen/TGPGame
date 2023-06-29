// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "UObject/Interface.h"
#include "Components/TimelineComponent.h"
#include "WeaponInterfaces.generated.h"

class UGunItem;

UINTERFACE(MinimalAPI)
class UCanHitScan : public UInterface
{
	GENERATED_BODY()
};

/**
* 
*/
class TGP_API ICanHitScan
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	public:
	bool DoRaycastReturnResult(UWorld* world, FHitResult& result, FVector startPoint, FVector endPoint, ECollisionChannel collisionChannel);
	
};

UINTERFACE(MinimalAPI)
class UHasAmmo : public UInterface
{
	GENERATED_BODY()
};

/**
* 
*/
class TGP_API IHasAmmo
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	public:
	int maxAmmo;
	int currentAmmoClip;
	int currentReserves;

	bool reloading;
	float reloadTime;

	bool infiniteReserves;

	FTimerHandle reloadTimerHandler;
	
	bool TryUseAmmo(AActor* actor, int amount);
	UFUNCTION()	virtual void StartReloadAmmo();
	void TryReload();
	UFUNCTION() virtual void ReloadEnded();
	void SetMaxAmmo(int ammo) { maxAmmo = ammo;}
	int GetAmmoCount() { return currentAmmoClip; }
	int GetReserves() { return currentReserves; }
	void AddAmmo(int amount) { currentReserves += amount; }
	virtual void CancelReload(AActor* actor);
};

UINTERFACE(MinimalAPI)
class UWaitTimer : public UInterface
{
	GENERATED_BODY()
};

/**
* 
*/
class TGP_API IWaitTimer
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	public:
	FTimerHandle waitTimeHandler;
	virtual void StartWaitTimer(AActor* actor, float time) {};
};

UENUM()
enum ERecoilDirection
{
	Forwards,
	Backwards
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUseRecoil : public UInterface
{
	GENERATED_BODY()
};

/**
* 
*/
class TGP_API IUseRecoil
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	public:
	FRotator originRotation; // Stores origin rotation when firing
	FRotator postRecoilRotation; // Stores rotation when finished firing
	bool startedRecoil; // Bool to show if recoil has begun
	FTimeline recoilTimeline; // Timeline to show recoil
	ERecoilDirection recoilTimelineDirection; // Enum to show which dir the recoil is moving
	bool singleFireRecoilStarted; // Single fire check for recoil
	float recoilMaxThreshhold; // Pre-calculated value of how high each curve can reach
	float notPlayedFullyValue; // Value that fixes an issue that results in inconsistent resets
	ERecoilDirection GetTimelineDirection() { return recoilTimelineDirection; }
	float AdjustRecoilForCompensate(); // Calculates how far the gun should move back down after recoil applied
	void EndRecoil(); // Ends recoil
	void ApplySingleFire(); // Starts Single Fire Checks
	void StartTimeline(); // Starts timeline
	void ReverseTimeline(float recoilModifier); // Starts timeline backwards
	bool HasStartedRecoil() { return startedRecoil; }
	void StartRecoil(FRotator startRot); // Starts Recoil and stores origin
	void ApplyRecoilPitch(APlayerController* controller, float Value, bool isSingleFire);
	void ApplyRecoilYaw(APlayerController* controller, float Value, bool isSingleFire);
};
