// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Interfaces/WeaponInterfaces.h"
#include "Item/BaseItem.h"
#include "Kismet/KismetMathLibrary.h"


// Add default functionality here for any IWeaponInterfaces functions that are not pure virtual.



bool ICanHitScan::DoRaycastReturnResult(UWorld* world, FHitResult& result, FVector startPoint, FVector endPoint, ECollisionChannel collisionChannel)
{
	bool DidTrace = world->LineTraceSingleByChannel(
			result,		//result
			startPoint,		//start
			endPoint,		//end
			collisionChannel	//collision channel
			);
	
	return DidTrace;
}


bool IHasAmmo::TryUseAmmo(AActor* actor, int amount)
{
	if(currentAmmoClip > 0)
	{
		if(reloading)
		{
			reloading = false;
			CancelReload(actor);
		}
		currentAmmoClip -= amount;
		// Set Ammo in "Weapon Item" to the new amount
		return true;
	}
	else
	{
		return false;
	}
}


void IHasAmmo::StartReloadAmmo()
{
	
	if(!reloading)
	{
		reloading = true;
		//UWorld* world = actor->GetWorld();
		ReloadEnded(); // DELETE WHEN TRYING TO FIX TIMER
		
		//FTimerDynamicDelegate eventTest;
		//eventTest.BindDynamic(this, &IHasAmmo::ReloadEnded);
		//world->GetTimerManager().SetTimer(reloadTimerHandler, eventTest, 5, true);
	}
}

void IHasAmmo::TryReload()
{
	if(currentReserves > 0)
	{
		StartReloadAmmo();
	}
} 

void IHasAmmo::ReloadEnded()
{
	if(reloading)
	{
		int neededClip = maxAmmo - currentAmmoClip;
		int newClip = FMath::Min(maxAmmo, currentReserves);
		int newValue = FMath::Min(newClip, neededClip);
		int addedBack = currentAmmoClip + newValue;
		if(!infiniteReserves)
		{
			currentReserves -= newValue;
		}

		currentAmmoClip = addedBack;
		reloading = false;


		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("WeaponReloaded"));
		// Set ammo in "WeaponItem"
	}
}

void IHasAmmo::CancelReload(AActor* actor)
{
	
}

float IUseRecoil::AdjustRecoilForCompensate()
{
	FRotator rotator = UKismetMathLibrary::NormalizedDeltaRotator(originRotation, postRecoilRotation);
	// Uncomment this to find max recoil threshhold
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Pitch: ") + FString::SanitizeFloat(rotator.Pitch));
	if(rotator.Pitch > 0)
	{
		return 0.0f;
	}
	float yAbs = FMath::Abs(rotator.Pitch);
	float afterMapped = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 32.245f), FVector2D(0.0f, 1.0f), yAbs);
	return afterMapped;
}

void IUseRecoil::EndRecoil()
{
	startedRecoil = false;
	recoilTimelineDirection = ERecoilDirection::Forwards;
	originRotation = FRotator();
	postRecoilRotation = FRotator();
	notPlayedFullyValue = 1.0f;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("End Recoil"));
}

void IUseRecoil::ApplySingleFire()
{
	singleFireRecoilStarted = true;
	recoilTimeline.SetNewTime(0);
	StartTimeline();
}

void IUseRecoil::StartTimeline()
{
	recoilTimeline.Play();
	recoilTimeline.SetPlayRate(1.0f);
	recoilTimelineDirection = ERecoilDirection::Forwards;
}

void IUseRecoil::ReverseTimeline(float recoilModifier)
{
	recoilTimeline.Reverse();
	recoilTimelineDirection = ERecoilDirection::Backwards;
	recoilTimeline.SetPlayRate(recoilModifier);
}

void IUseRecoil::StartRecoil(FRotator startRot)
{
	startedRecoil = true;
	originRotation = startRot;
}

void IUseRecoil::ApplyRecoilPitch(APlayerController* controller, float Value, bool isSingleFire)
{
	if(GetTimelineDirection() == ERecoilDirection::Forwards)
	{
		Value *= -1;
	}
	else if(GetTimelineDirection() == ERecoilDirection::Backwards && !isSingleFire)
	{
		Value *= AdjustRecoilForCompensate();
	}

	Value = Value * recoilTimeline.GetPlayRate() * notPlayedFullyValue;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Pitch Increase: ") + FString::SanitizeFloat(Value));
	if(controller)
		controller->AddPitchInput(Value * controller->GetWorld()->GetDeltaSeconds());
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Valid Controller"));
	}
}

void IUseRecoil::ApplyRecoilYaw(APlayerController* controller, float Value, bool isSingleFire)
{
	if(GetTimelineDirection() == ERecoilDirection::Forwards)
	{
		Value *= -1;
	}
	else if(GetTimelineDirection() == ERecoilDirection::Backwards && !isSingleFire)
	{
		Value *= AdjustRecoilForCompensate();
	}
	Value = Value * recoilTimeline.GetPlayRate() * notPlayedFullyValue;
	if(controller)
		controller->AddYawInput(Value * controller->GetWorld()->GetDeltaSeconds());
}
