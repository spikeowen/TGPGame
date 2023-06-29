// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/WeaponInterfaces.h"
#include "Weapons/WeaponComponent.h"
#include "ProjectileWeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TGP_API UProjectileWeaponComponent : public UWeaponComponent, public IWaitTimer, public IHasAmmo, public IUseRecoil
{
	GENERATED_BODY()

protected:
	virtual void StartReloadAmmo() override;
	virtual void StartWaitTimer(AActor* actor, float time) override;
	virtual void CancelReload(AActor* actor) override;

	UFUNCTION() void RecoilTimelineProgressPitch(float Value);
	UFUNCTION() void RecoilTimelineProgressYaw(float Value);
	UFUNCTION() void RecoilTimelineFinished();
	UFUNCTION() void SingleFireRecoilReset();

	void ResetRecoilTimeline();
	
    UPROPERTY(EditAnywhere) TSubclassOf<class AMyDamageMarker> _damageMarker;

	UPROPERTY() class UCurveFloat* _curve;
public:
	UProjectileWeaponComponent();

	
	virtual void BeginPlay() override;
	virtual bool OnFire() override;
	virtual void OnFireEnd() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void InitializeWeapon(UGunItem* gunItem) override;
	virtual void DropWeapon() override;
	virtual FVector2D GetCurrentAmmo() override;
};
