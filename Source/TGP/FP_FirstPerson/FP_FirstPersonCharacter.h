// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interactables/IInteractable.h"
#include "Item/ItemInfo.h"
#include "FP_FirstPersonCharacter.generated.h"

class UInputComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class USoundBase;
class UAnimMontage;
class UWeaponItem;
class UPlayerInventory;
class AGrenadeWeapon;
class UWeaponComponent;

UCLASS(config=Game)
class AFP_FirstPersonCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FP_Gun;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UPlayerInventory* PlayerInventory;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UWeaponComponent* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USceneComponent* AimOffset;

public:
	AFP_FirstPersonCharacter();

	UPlayerInventory* GetInventory() const { return PlayerInventory; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	UCapsuleComponent* SwordCollider;
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* CombatMontage;

	/* This is when calculating the trace to determine what the weapon has hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float WeaponRange;
	
	/* This is multiplied by the direction vector when the weapon trace hits something to apply velocity to the component that is hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float WeaponDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	FVector WeaponAimLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	FVector WeaponDefaultLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	FRotator WeaponDefaultRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	FRotator MeshDefaultRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	FRotator AimRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	float WeaponYawDiff;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	float WeaponPitchDiff;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	bool IsSprinting;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	bool IsAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool IsReloading;

	bool CanFire;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Combat")
	bool IsMeleeAttacking;
protected:

	void OnFireWeapon();

	void OnFireWeaponRelease();
	
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	void ChangeWeapon(float Val);


	/* 
	 * Performs a trace between two points
	 * 
	 * @param	StartTrace	Trace starting point
	 * @param	EndTrac		Trace end point
	 * @returns FHitResult returns a struct containing trace result - who/what the trace hit etc.
	 */
	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true")) class UHealthComponent* _healthComponent;
	
	bool _fireHeld;
	
	void PickupWeapon();
	void DropWeapon();
	void ThrowUtility();
	
	bool _weaponQueued;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
	UFUNCTION() void OnWeaponChanged(UWeaponItem* WeaponItem);
	
	UFUNCTION(Server, Reliable) void SrvShootGun();
	void SrvShootGun_Implementation();
	
	UFUNCTION(Server, Reliable) void OnChangeSelectedWeapon(int Slot);
	void OnChangeSelectedWeapon_Implementation(int Slot);

	UFUNCTION(Server, Reliable) void OnPickUpItem(class AItemActor* ItemActor, int Slot);
	void OnPickUpItem_Implementation(AItemActor* ItemActor, int Slot);
	
	UFUNCTION(NetMulticast, Reliable) void ChangeWeaponMeshMulti(int ItemId);
	void ChangeWeaponMeshMulti_Implementation(int ItemId);

	UFUNCTION(Server, Reliable) void RequestWeaponMeshChange(int Slot);
	void RequestWeaponMeshChange_Implementation(int Slot);
	
	UPROPERTY() AActor* _lastLooked;
	IIInteractable* _lastLookedInterface;
	void InteractWithObject();
	
	void CastForInteractable(float DeltaTime);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = true)) TSubclassOf<AGrenadeWeapon> _grenadeToSpawn;

	UFUNCTION() void OnOverlapWithActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera", meta=(AllowPrivateAccess = true))
	float M_DefaultCameraSensitivity;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera", meta=(AllowPrivateAccess = true))
	float M_CameraSensitivity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera", meta=(AllowPrivateAccess = true))
	float M_AimSensitivity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera", meta=(AllowPrivateAccess = true))
	float M_SniperSensitivity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement", meta=(AllowPrivateAccess = true))
	float M_DefaultSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement", meta=(AllowPrivateAccess = true))
	float M_SprintSpeed;

	bool IsMovingForward = false;
	bool IsMovingRight = false;

	FVector M_RifleAimOffset;
	FVector M_PistolAimoffset;

	UFUNCTION(BlueprintCallable)
	UWeaponItem* GetUnusedItem();

	UPROPERTY()
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Audio", meta=(AllowPrivateAccess = true))
	class USoundCue* ReloadSound;
public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintImplementableEvent) void TestDebug();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UWeaponComponent* GetCurrentWeaponComponent() { return WeaponComponent; }

	void ReloadWeapon();
	
	void LookUp(float inputValue);
	void Turn(float inputValue);

	UFUNCTION(BlueprintImplementableEvent)
	void SetAnimation();
	
	void AttachWeapon();

	void Sprint();
	void StopSprint();

	void NewAim();
	UFUNCTION(BlueprintCallable)
	void NewStopAim();
	UFUNCTION(BlueprintCallable)
	void ResetAim();
	UFUNCTION(BlueprintImplementableEvent)
	void ToggleCrosshair(bool OnOff);
 
	UFUNCTION(BlueprintCallable)
	void SwitchWeapon();

	UFUNCTION(BlueprintCallable)
	void Reload();

	void PlayFireAnim();

	UFUNCTION(BlueprintCallable)
	void SwordColliderOn();
	UFUNCTION(BlueprintCallable)
	void SwordColliderOff();

	UFUNCTION(BlueprintImplementableEvent)
	void AdjustToSight();

	UFUNCTION(BlueprintImplementableEvent)
	void AdjustToIrons();

	UFUNCTION()
	void MeleeDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void TriggerHealthUpdate();
	void TriggerPrimaryIconUpdate();
	void TriggerSecondaryIconUpdate();
	void TriggerCrosshairToggle(bool Hidden);
	void TriggerSniperToggle(bool Hidden);
	void TriggerRarityUpdate();

	void SetHUD();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ToggleSniperScopeVisibility(bool Visible);

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleIronSightVisiblity(bool Visible);
};



