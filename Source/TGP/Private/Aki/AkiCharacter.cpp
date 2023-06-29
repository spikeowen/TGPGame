// Fill out your copyright notice in the Description page of Project Settings.


#include "Aki/AkiCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
AAkiCharacter::AAkiCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetRootComponent());

	GetMesh()->SetupAttachment(Camera);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	WeaponMesh->SetupAttachment(GetMesh());

	M_CameraSensitivity = 0.6f; //Try keep between 0-1 otherwise sensitivity gets out of hand

	IsSprinting = false;
	M_DefaultSpeed = 600.0f;
	M_SprintSpeed = 1350.0f;

	IsAiming = false;
	IsReloading = false;

	M_GunReloadSpeed = 2.5f;
	
}

// Called when the game starts or when spawned
void AAkiCharacter::BeginPlay()
{
	Super::BeginPlay();

	AttachWeapon();
}

// Called every frame
void AAkiCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAkiCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Camera Controls
	PlayerInputComponent->BindAxis("LookUp", this, &AAkiCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &AAkiCharacter::Turn);

	//PlayerMovement
	PlayerInputComponent->BindAxis("MoveForward", this, &AAkiCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAkiCharacter::MoveRight);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AAkiCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AAkiCharacter::StopSprint);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AAkiCharacter::BeginAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AAkiCharacter::EndAim);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AAkiCharacter::Reload);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AAkiCharacter::Attack);

}

void AAkiCharacter::LookUp(float inputValue)
{
	FRotator CameraRotation = Camera->GetComponentRotation();
	CameraRotation.Pitch = FMath::Clamp(CameraRotation.Pitch - (inputValue * M_CameraSensitivity), -60.0f, 60.0f);

	Camera->SetWorldRotation(CameraRotation);
}

void AAkiCharacter::Turn(float inputValue)
{
	AddControllerYawInput(inputValue * M_CameraSensitivity);
}

void AAkiCharacter::MoveForward(float inputValue)
{
	GetMovementComponent()->AddInputVector(GetActorForwardVector() * inputValue);
}

void AAkiCharacter::MoveRight(float inputValue)
{
	GetMovementComponent()->AddInputVector(GetActorRightVector() * inputValue);
}

void AAkiCharacter::Sprint()
{
	if(!IsReloading)
	{
		IsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = M_SprintSpeed;
	}
}

void AAkiCharacter::StopSprint()
{
	IsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = M_DefaultSpeed;
}

void AAkiCharacter::BeginAim()
{
	if(WeaponType != 2)
	{
		IsAiming = true;
		if(WeaponMesh->SkeletalMesh->GetName().Contains("Sniper"))
		{
			Camera->SetFieldOfView(60.0f);
		}
		else
		{
			Camera->SetFieldOfView(85.0f);
		}		
		Aim();
	}
}

void AAkiCharacter::EndAim()
{
	if(WeaponType != 2)
	{
		IsAiming = false;
		Camera->SetFieldOfView(100.0f);
		StopAim();
	}
}

void AAkiCharacter::Reload()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if(AnimInstance)
	{
		IsReloading = true;
		StopSprint();
		AnimInstance->Montage_Play(CombatMontage, M_GunReloadSpeed);
		AnimInstance->Montage_JumpToSection(FName("RifleReload"), CombatMontage);
	}
}

void AAkiCharacter::ReloadFinished()
{
	IsReloading = false;
}

void AAkiCharacter::AttachWeapon()
{
	if(WeaponMesh->SkeletalMesh->GetName().Contains("Rifle") || WeaponMesh->SkeletalMesh->GetName().Contains("SMG") || WeaponMesh->SkeletalMesh->GetName().Contains("Shotgun") || WeaponMesh->SkeletalMesh->GetName().Contains("Sniper"))
	{
		if(WeaponMesh->SkeletalMesh->GetName().Contains("SMG"))
		{
			WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("SubMachineSocket"));
		}
		else
		{
			WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("RifleSocket"));
		}
		WeaponType = 0;
		SetAnimation();
	}
	else if(WeaponMesh->SkeletalMesh->GetName().Contains("Pistol") || WeaponMesh->SkeletalMesh->GetName().Contains("Revolver"))
	{
		WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("PistolSocket"));
		WeaponType = 1;
		SetAnimation();
	}
	else if(WeaponMesh->SkeletalMesh->GetName().Contains("Sword"))
	{
		WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("MeleeSocket"));
		WeaponType = 2;
		SetAnimation();
	}

	SetWeaponTransformDefaults();
}

void AAkiCharacter::SetWeaponTransformDefaults()
{
	WeaponDefaultLocation = GetMesh()->GetRelativeLocation();
	WeaponLocationOffset = Camera->GetComponentLocation() - WeaponMesh->GetSocketLocation(FName("AimSocket"));
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, WeaponLocationOffset.ToString());
	const float TempY = WeaponLocationOffset.Y;
	WeaponLocationOffset.Y = WeaponLocationOffset.X;
	WeaponLocationOffset.X = TempY;

	WeaponLocationOffset = WeaponDefaultLocation + WeaponLocationOffset;
	WeaponLocationOffset.Y += 7.5f;

	MeshDefaultRotation = GetMesh()->GetRelativeRotation();

	WeaponDefaultRotation = WeaponMesh->GetComponentRotation();
	WeaponDefaultRotation.Yaw += 90.0;

	WeaponYawDiff = Camera->GetComponentRotation().Yaw - WeaponDefaultRotation.Yaw;
	WeaponAimYaw = GetMesh()->GetRelativeRotation().Yaw + WeaponYawDiff;
}

void AAkiCharacter::Attack()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if(!AnimInstance)
	{
		return;
	}
	
	if(WeaponMesh->SkeletalMesh->GetName().Contains("Sword") && !AnimInstance->Montage_IsPlaying(CombatMontage))
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Melee2"), CombatMontage);
	}
}




