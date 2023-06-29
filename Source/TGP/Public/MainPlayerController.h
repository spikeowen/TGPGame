// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TGP_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HUD")
	TSubclassOf<class UUserWidget> HUDBlueprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HUD")
	UUserWidget* DisplayedHUD;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HUD")
	UTexture2D* PrimaryIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HUD")
	UTexture2D* SecondaryIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HUD")
	FString SpawnRegion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HUD")
	FString PlayerRegion;
	
protected:
	virtual  void BeginPlay() override;
	virtual  void OnPossess(APawn* InPawn) override;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHealth(float HealthPercent);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdatePrimaryWeapon();
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateSecondaryWeapon();
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateEnemyRegion();

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleCrosshair(bool hidden);

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleSniperScope(bool hidden);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWeaponRarity(int rarity);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateSecondWeaponRarity(int rarity);
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCurrentZone();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCurrentAmmo(int ammo);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateReserveAmmo(int ReserveAmmo);

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleAmmoDisplay(bool Hidden);
};
