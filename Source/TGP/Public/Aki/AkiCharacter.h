// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AkiCharacter.generated.h"

UCLASS()
class TGP_API AAkiCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAkiCharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera")
	class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Mesh")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Combat")
	class UAnimMontage* CombatMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	FVector WeaponLocationOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	FVector WeaponDefaultLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	FRotator WeaponDefaultRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	FRotator MeshDefaultRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	float WeaponYawDiff;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	float WeaponAimYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	bool IsSprinting;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	bool IsAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool IsReloading;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation")
	int WeaponType;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera", meta=(AllowPrivateAccess = true))
	float M_CameraSensitivity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement", meta=(AllowPrivateAccess = true))
	float M_DefaultSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement", meta=(AllowPrivateAccess = true))
	float M_SprintSpeed;

	float M_GunReloadSpeed;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void LookUp(float inputValue);
	void Turn(float inputValue);

	void MoveForward(float inputValue);
	void MoveRight(float inputValue);

	void Sprint();
	void StopSprint();

	UFUNCTION(BlueprintImplementableEvent)
	void Aim();

	UFUNCTION(BlueprintImplementableEvent)
	void StopAim();

	void BeginAim();
	void EndAim();

	void Reload();

	UFUNCTION(BlueprintCallable)
	void ReloadFinished();

	UFUNCTION(BlueprintImplementableEvent)
	void SetAnimation();

	void AttachWeapon();

	UFUNCTION(BlueprintCallable)
	void SetWeaponTransformDefaults();

	void Attack();
};


