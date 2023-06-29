#include "FP_FirstPersonCharacter.h"

#include "DrawDebugHelpers.h"
#include "MainPlayerController.h"
#include "Ai/BaseAiCharacter.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/ActorChannel.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameInstance/BaseGameInstance.h"
#include "Weapons/GunHostActor.h"
#include "Weapons/WeaponComponent.h"
#include "Weapons/Interfaces/WeaponInterfaces.h"
#include "Inventory/PlayerInventory.h"
#include "Item/BaseItem.h"
#include "Sound/SoundCue.h"
#include "Item/ItemActor.h"
#include "Item/ItemInfo.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/HitscanWeaponComponent.h"
#include "Weapons/Throwables/GrenadeWeapon.h"
#include "Weapons/HealthComponent.h"
#include "Weapons/Projectiles/Projectile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapons/Throwables/ThrowableWeapon.h"

#define COLLISION_WEAPON		ECC_GameTraceChannel1

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

AFP_FirstPersonCharacter::AFP_FirstPersonCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0, 0, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);				// Set so only owner can see mesh
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);	// Attach mesh to FirstPersonCameraComponent
	Mesh1P->bCastDynamicShadow = false;			// Disallow mesh to cast dynamic shadows
	Mesh1P->CastShadow = false;				// Disallow mesh to cast other shadows

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// Only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;		// Disallow mesh to cast dynamic shadows
	FP_Gun->CastShadow = false;			// Disallow mesh to cast other shadows
	FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));

	WeaponComponent = CreateDefaultSubobject<UHitscanWeaponComponent>(TEXT("GunComponent"));
	AddOwnedComponent(WeaponComponent);

	PlayerInventory = CreateDefaultSubobject<UPlayerInventory>(TEXT("PlayerInv"));
	AddOwnedComponent(PlayerInventory);

	_healthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	AddOwnedComponent(_healthComponent);

	// Set weapon damage and range
	WeaponRange = 5000.0f;
	WeaponDamage = 500000.0f;

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	M_DefaultSpeed = 600.0f;
	M_SprintSpeed = 1350.0f;

	M_DefaultCameraSensitivity = 0.6;
	M_SniperSensitivity = 3.25;
	M_AimSensitivity = 1.5;
	M_CameraSensitivity = M_DefaultCameraSensitivity;

	CanFire = true;

	SwordCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Sword Collider"));
	SwordCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SwordCollider->SetCollisionResponseToAllChannels(ECR_Ignore);

	AimOffset = CreateDefaultSubobject<USceneComponent>(TEXT("Aim Weapon Location"));
	AimOffset->SetupAttachment(FirstPersonCameraComponent);

	IsReloading = false;

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFP_FirstPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);
	
	// Set up gameplay key bindings

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//Bind Sprint events
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AFP_FirstPersonCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint",  IE_Released,  this, &AFP_FirstPersonCharacter::StopSprint);

	//Bind Aim events
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AFP_FirstPersonCharacter::NewAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AFP_FirstPersonCharacter::NewStopAim);
	
	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFP_FirstPersonCharacter::OnFireWeapon);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFP_FirstPersonCharacter::OnFireWeaponRelease);
	PlayerInputComponent->BindAction("Pickup", IE_Pressed, this, &AFP_FirstPersonCharacter::InteractWithObject);
	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &AFP_FirstPersonCharacter::DropWeapon);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFP_FirstPersonCharacter::ReloadWeapon);
	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &AFP_FirstPersonCharacter::ThrowUtility);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFP_FirstPersonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFP_FirstPersonCharacter::MoveRight);
	
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &AFP_FirstPersonCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this,&AFP_FirstPersonCharacter::LookUp);

	PlayerInputComponent->BindAxis("ChangeWeapon", this, &AFP_FirstPersonCharacter::ChangeWeapon);
}

void AFP_FirstPersonCharacter::ChangeWeapon(float Val)
{
	if (Val != 0.f)
	{
		IsMeleeAttacking = false;
		IsReloading = false;
		CanFire = true;
		
		if ((int)Val - 1 != PlayerInventory->GetSelectedWeaponSlot())
		{
			ResetAim();
			PlayerInventory->ChangeWeapon(EWeaponSlot(Val - 1.f));
			OnChangeSelectedWeapon(PlayerInventory->GetSelectedWeaponSlot());
		}
	}
}


void AFP_FirstPersonCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		IsMovingForward = true;
		// Add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
	else
	{
		if(!IsMovingRight)
		{
			StopSprint();
		}
	}
}

void AFP_FirstPersonCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		IsMovingRight = true;
		// Add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
	else
	{
		IsMovingRight = false;
	}
}

FHitResult AFP_FirstPersonCharacter::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);

	return Hit;
}

void AFP_FirstPersonCharacter::OnFireWeapon()
{
	if(WeaponComponent->GetWeaponInfo()->WeaponType == EWeaponType::Sword)
	{
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();

		if(!AnimInstance)
		{
			return;
		}

		if(IsMeleeAttacking)
		{
			return;
		}
			
		IsMeleeAttacking = true;
		// int AnimToPlay = FMath::RandRange(0,1);
		//
		// if(AnimToPlay == 0)
		// {
		// 	AnimInstance->Montage_Play(CombatMontage, 1.0f);
		// 	AnimInstance->Montage_JumpToSection("Melee1", CombatMontage);
		// }
		// else
		// {
		// 	AnimInstance->Montage_Play(CombatMontage, 1.0f);
		// 	AnimInstance->Montage_JumpToSection("Melee2", CombatMontage);
		// }
		
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
       	AnimInstance->Montage_JumpToSection("Melee1", CombatMontage);
		
		return;
	}
	_fireHeld = true;
}

void AFP_FirstPersonCharacter::OnFireWeaponRelease()
{
	_fireHeld = false;
	WeaponComponent->OnFireEnd();
}

void AFP_FirstPersonCharacter::PickupWeapon()
{
	WeaponComponent->PickupWeapon(this);
	FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
}

void AFP_FirstPersonCharacter::DropWeapon()
{
	if (PlayerInventory->GetSelectedWeaponSlot() == EWeaponSlot::Melee)
	{
		return;
	}
	
	WeaponComponent->DropWeapon();
	PlayerInventory->SrvDropWeapon(PlayerInventory->GetSelectedWeaponSlot());
	//PlayerInventory->DropWeapon();
}

void AFP_FirstPersonCharacter::ReloadWeapon()
{
	if(!WeaponComponent || IsReloading)
	{
		return;
	}
	
	if(WeaponComponent->GetWeaponInfo()->WeaponType == EWeaponType::Sword)
		return;
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Start Reload"));
	StopSprint();

	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
	if(AnimInstance && WeaponComponent->GetCurrentAmmo().Y > 0)
	{
		IsReloading = true;
		float ReloadID = CombatMontage->GetSectionIndex("Reload");
		float ReloadRawLength = CombatMontage->GetSectionLength(ReloadID);
		float AdjustedTime = ReloadRawLength / WeaponComponent->GetWeaponInfo()->ReloadSpeed;
		AnimInstance->Montage_Play(CombatMontage, AdjustedTime);
		AnimInstance->Montage_JumpToSection("Reload", CombatMontage);
	}
}

void AFP_FirstPersonCharacter::ThrowUtility()
{
	UThrowableItem* Throwable = PlayerInventory->GetSelectedUtility();

	if (Throwable)
	{
		APlayerController* _playerController = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0);
		FVector pos;
		FRotator rot;
		_playerController->GetPlayerViewPoint(pos, rot);
	
		AProjectile* ThrowableActor = GetWorld()->SpawnActor<AProjectile>(Cast<UThrowableInfo>(Throwable->GetItemInfo())->ThrowableBlueprint, GetActorLocation() + FVector(0.0f, 0.0f, 50.0f) + rot.Vector() * 100.0f, FRotator());
		ThrowableActor->Initialize(Cast<UThrowableInfo>(Throwable->GetItemInfo()));
		ThrowableActor->SetProjectileParameters(_playerController, rot.Vector(), 100000.0f);

		PlayerInventory->OnUseUtility();
	}
}

void AFP_FirstPersonCharacter::OnWeaponChanged(UWeaponItem* WeaponItem)
{
	if (WeaponItem != nullptr)
		FP_Gun->SetSkeletalMesh(Cast<UWeaponInfo>(WeaponItem->GetItemInfo())->WeaponSkeletalMesh);
	else
		FP_Gun->SetSkeletalMesh(nullptr);
	
	//if (HasAuthority())
	//{
	//	WeaponComponent->DropWeapon();
	//	
	//	WeaponComponent = NewObject<UWeaponComponent>(this, Cast<UGunInfo>(WeaponItem->GetItemInfo())->BaseWeaponClass, FName(WeaponItem->GetItemInfo()->ItemName));
	//	WeaponComponent->RegisterComponentWithWorld(GetWorld());
//
	//	WeaponComponent->PickupWeapon(this); // Assign player to component
	//	WeaponComponent->SetParentMesh(FP_Gun);
	//}
	
	if (UGunItem* Gun = Cast<UGunItem>(WeaponItem))
		WeaponComponent->InitializeWeapon(Gun);

	RequestWeaponMeshChange(PlayerInventory->GetSelectedWeaponSlot());
	AttachWeapon();
	
	TriggerPrimaryIconUpdate();
	TriggerSecondaryIconUpdate();
	TriggerRarityUpdate();

	if(MainPlayerController)
	{
		if(WeaponComponent->GetWeaponInfo()->WeaponType == Sword)
		{
			MainPlayerController->ToggleAmmoDisplay(true);
		}
		else
		{
			MainPlayerController->ToggleAmmoDisplay(false);
			MainPlayerController->UpdateCurrentAmmo(WeaponComponent->GetCurrentAmmo().X);
			MainPlayerController->UpdateReserveAmmo(WeaponComponent->GetCurrentAmmo().Y);
		}
	}
}


void AFP_FirstPersonCharacter::SrvShootGun_Implementation()
{
	//WeaponComponent->SrvOnFire();
}

void AFP_FirstPersonCharacter::OnChangeSelectedWeapon_Implementation(int Slot)
{
	PlayerInventory->ChangeWeapon((EWeaponSlot)Slot, true, false);
}

void AFP_FirstPersonCharacter::OnPickUpItem_Implementation(AItemActor* ItemActor, int Slot)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::FromInt(Slot));
	
	if(PlayerInventory->TryPickUpItem(ItemActor->GetItem(), Slot))
		ItemActor->Destroy();
}

 
void AFP_FirstPersonCharacter::ChangeWeaponMeshMulti_Implementation(int ItemId)
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
		return;
	
	if (UBaseGameInstance* GI = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
		if (UWeaponInfo* Info = Cast<UWeaponInfo>(GI->FindInfoUniqueId(ItemId)))
			FP_Gun->SetSkeletalMesh(Info->WeaponSkeletalMesh);
}

void AFP_FirstPersonCharacter::RequestWeaponMeshChange_Implementation(int Slot)
{
	PlayerInventory->ChangeWeapon((EWeaponSlot)Slot, true, false);

	if (UWeaponItem* Wep = PlayerInventory->GetSelectedWeapon())
	{
		ChangeWeaponMeshMulti(Wep->GetItemId());

		if (UGunItem* Gun = Cast<UGunItem>(Wep))
		{
			WeaponComponent = NewObject<UWeaponComponent>(this, Cast<UGunInfo>(Gun->GetItemInfo())->BaseWeaponClass, FName(Gun->GetItemInfo()->ItemName));
			WeaponComponent->RegisterComponentWithWorld(GetWorld());

			WeaponComponent->PickupWeapon(this); // Assign player to component
			WeaponComponent->SetParentMesh(FP_Gun);
			WeaponComponent->InitializeWeapon(Gun);
		}
	}
}

void AFP_FirstPersonCharacter::InteractWithObject()
{
	if(_lastLooked != nullptr)
	{
		if(_lastLooked->ActorHasTag(TEXT("Weapon")))
		{
			if (AItemActor* ItemActor = Cast<AItemActor>(_lastLooked))
					OnPickUpItem(ItemActor, PlayerInventory->GetSelectedWeaponSlot());
		}
		else if(_lastLookedInterface)
		{
			_lastLookedInterface->Interact(this);
		}
	}
}

void AFP_FirstPersonCharacter::CastForInteractable(float DeltaTime)
{
	const FVector CameraPos = GetFirstPersonCameraComponent()->GetComponentLocation(), CameraDir = GetFirstPersonCameraComponent()->GetForwardVector();
	
	FHitResult Result;
	if (GetWorld()->LineTraceSingleByChannel(Result, CameraPos, CameraPos + (CameraDir * 250.f), ECollisionChannel::ECC_Visibility))
	{
		if(_lastLooked != Result.GetActor())
		{
			if(_lastLookedInterface != nullptr)
				_lastLookedInterface->EndHover();
			_lastLooked = Result.GetActor();
			if (IIInteractable* InteractActor = Cast<AItemActor>(Result.Actor))
			{
				_lastLookedInterface = InteractActor;
				InteractActor->StartHover();
			}
		}
	}
	else
	{
		_lastLooked = nullptr;
		if(_lastLookedInterface != nullptr)
			_lastLookedInterface->EndHover();
		_lastLookedInterface = nullptr;
	}
}

void AFP_FirstPersonCharacter::OnOverlapWithActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OVERLAP EVENT"));
}


UWeaponItem* AFP_FirstPersonCharacter::GetUnusedItem()
{
	EWeaponSlot currentSlot = PlayerInventory->GetSelectedWeaponSlot();
	EWeaponSlot slotToGet;
	if(currentSlot == EWeaponSlot::Primary)
		slotToGet = EWeaponSlot::Secondary;
	else
		slotToGet = EWeaponSlot::Primary;

	return PlayerInventory->GetWeaponAtSlot(slotToGet);
}

void AFP_FirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	ToggleIronSightVisiblity(true);
	ToggleSniperScopeVisibility(true);
	
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AFP_FirstPersonCharacter::OnOverlapWithActor);

	//_currentWeapon = Cast<AGunHostActor>(GunActorComponent->GetChildActor());
	//_currentWeaponComponent = _currentWeapon->GetWeaponComponent();
	WeaponComponent->SetParentMesh(FP_Gun);
	WeaponComponent->PickupWeapon(this);
		
	//_currentWeapon = nullptr;
	//_currentWeaponComponent = nullptr;

	if (PlayerInventory)
	{
		PlayerInventory->OnWeaponChangedEvent.AddDynamic(this, &AFP_FirstPersonCharacter::OnWeaponChanged);
		PlayerInventory->ComponentLoadComplete();
	}
	
	if (HasAuthority())
	{
		_fireHeld = false;
	}

	_lastLooked = nullptr;

	SwordCollider->OnComponentBeginOverlap.AddDynamic(this, &AFP_FirstPersonCharacter::MeleeDamage);

	if(GetController()->IsA(AMainPlayerController::StaticClass()))
	{
		MainPlayerController = Cast<AMainPlayerController>(GetController());
	}
	if(MainPlayerController)
	{
		if(WeaponComponent->GetWeaponInfo()->WeaponType == EWeaponType::Sword)
		{
			MainPlayerController->ToggleAmmoDisplay(true);
		}
		else
		{
			MainPlayerController->UpdateCurrentAmmo(WeaponComponent->GetCurrentAmmo().X);
			MainPlayerController->UpdateReserveAmmo(WeaponComponent->GetCurrentAmmo().Y);
		}
	}

	ChangeWeapon(2);
	ChangeWeapon(1);
}

void AFP_FirstPersonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AFP_FirstPersonCharacter, _currentWeapon);
	//DOREPLIFETIME(AFP_FirstPersonCharacter, _currentWeaponComponent);
	//DOREPLIFETIME(AFP_FirstPersonCharacter, _fireHeld);
}

bool AFP_FirstPersonCharacter::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bUpdate = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	//bUpdate |= Channel->ReplicateSubobject(_currentWeapon, *Bunch, *RepFlags);
	//bUpdate |= Channel->ReplicateSubobject(WeaponComponent, *Bunch, *RepFlags);
 
	return bUpdate;
}

void AFP_FirstPersonCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (Controller)
	{
		CastForInteractable(DeltaSeconds);

		if(_fireHeld)
		{
			//if (HasAuthority())
			//	WeaponComponent->SrvOnFire();
			//else
			if(WeaponComponent->OnFire())
				IsReloading = false;
		}
	}
	
	//if(_fireHeld && _currentWeapon != nullptr)
	//{
	//	_currentWeaponComponent->OnFire();
	//	
	//	if (_currentWeaponComponent->bDidFire)
	//	{
	//		//SrvHitScan();
	//	}
	//}
}

void AFP_FirstPersonCharacter::LookUp(float inputValue)
{
	AddControllerPitchInput(inputValue * M_CameraSensitivity);
}

void AFP_FirstPersonCharacter::Turn(float inputValue)
{
	AddControllerYawInput(inputValue * M_CameraSensitivity);
}

void AFP_FirstPersonCharacter::AttachWeapon()
{
	switch (WeaponComponent->GetWeaponInfo()->WeaponType)
	{
	case EWeaponType::OneHand:
		{
			FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("PistolSocket"));
			if(FP_Gun->SkeletalMesh->GetName().Contains("Machine"))
			{
				ToggleIronSightVisiblity(true);
				ToggleSniperScopeVisibility(false);
			}
			else
			{
				ToggleIronSightVisiblity(false);
				ToggleSniperScopeVisibility(false);
			}
			break;
		}
	case EWeaponType::TwoHand:
		{
			if(FP_Gun->SkeletalMesh->GetName().Contains("SMG"))
			{
				FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("SubMachineSocket"));
				ToggleIronSightVisiblity(false);
				ToggleSniperScopeVisibility(false);
			}
			else
			{
				FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("RifleSocket"));
				if(FP_Gun->SkeletalMesh->GetName().Contains("Sniper"))
				{
					ToggleIronSightVisiblity(false);
					ToggleSniperScopeVisibility(true);
					break;
				}
				if(FP_Gun->SkeletalMesh->GetName().Contains("SMG"))
				{
					ToggleIronSightVisiblity(false);
					ToggleSniperScopeVisibility(false);
					break;
				}
				ToggleIronSightVisiblity(true);
				ToggleSniperScopeVisibility(false);
				
			}
			break;
		}
	case EWeaponType::Sword:
		{
			FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("MeleeSocket"));
			SwordCollider->AttachToComponent(FP_Gun, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Collider"));
			break;
		}
	default:
		break;
	}
	SetAnimation();
}

void AFP_FirstPersonCharacter::Sprint()
{
	if(!IsReloading && GetVelocity().Size() != 0)
	{
		IsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = M_SprintSpeed;
	}
}

void AFP_FirstPersonCharacter::StopSprint()
{
	IsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = M_DefaultSpeed;
}

void AFP_FirstPersonCharacter::NewAim()
{
	if(WeaponComponent->GetWeaponInfo()->WeaponType == EWeaponType::Sword)
	{
		if(IsAiming)
		{
			NewStopAim();
		}
		
		return;
	}
	
	IsAiming = true;
	WeaponComponent->SetADS(true);
	if(FP_Gun->SkeletalMesh->GetName().Contains("Sniper"))
	{
		M_CameraSensitivity = M_DefaultCameraSensitivity/M_SniperSensitivity;
		FirstPersonCameraComponent->SetFieldOfView(25.0f);
	}
	else
	{
		M_CameraSensitivity = M_DefaultCameraSensitivity/M_AimSensitivity;
		FirstPersonCameraComponent->SetFieldOfView(85.0f);
	}
	TriggerCrosshairToggle(true);
	Mesh1P->SetHiddenInGame(true);
	FP_Gun->AttachToComponent(AimOffset, FAttachmentTransformRules::SnapToTargetIncludingScale);
	
	switch (WeaponComponent->GetWeaponInfo()->WeaponType)
	{
	case EWeaponType::TwoHand:
		if(FP_Gun->SkeletalMesh->GetName().Contains("SMG"))
		{
			AdjustToIrons();
			break;
		}

		if(FP_Gun->SkeletalMesh->GetName().Contains("Sniper"))
		{
			TriggerSniperToggle(false);
		}
		
		AdjustToSight();
		break;
	case EWeaponType::OneHand:
		
		if(FP_Gun->SkeletalMesh->GetName().Contains("Machine"))
		{
			AdjustToSight();
			break;
		}
		
		AdjustToIrons();
		break;
	default:
		break;
	}
}

void AFP_FirstPersonCharacter::NewStopAim()
{
	if(IsAiming)
	{
		if(FP_Gun->SkeletalMesh->GetName().Contains("Sniper"))
		{
			TriggerSniperToggle(true);
		}

		WeaponComponent->SetADS(false);
		
		TriggerCrosshairToggle(false);
		Mesh1P->SetHiddenInGame(false);
		IsAiming = false;
		M_CameraSensitivity = M_DefaultCameraSensitivity;
		FirstPersonCameraComponent->SetFieldOfView(100.0f);
		AttachWeapon();
	}
}

void AFP_FirstPersonCharacter::ResetAim()
{
	if(FP_Gun->SkeletalMesh->GetName().Contains("Sniper"))
	{
		TriggerSniperToggle(true);
	}
	
	TriggerCrosshairToggle(false);
	Mesh1P->SetHiddenInGame(false);
	IsAiming = false;
	M_CameraSensitivity = M_DefaultCameraSensitivity;
	FirstPersonCameraComponent->SetFieldOfView(100.0f);
}

void AFP_FirstPersonCharacter::SwitchWeapon()
{
	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();

	if(!AnimInstance)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Orange, TEXT("NoAnimations"));
		return;
	}
	
	switch (WeaponComponent->GetWeaponInfo()->WeaponType)
	{
	case EWeaponType::TwoHand:
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection("SwitchRifle", CombatMontage);
		break;
	case EWeaponType::OneHand:
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection("SwitchPistol", CombatMontage);
		break;
	case EWeaponType::Sword:
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection("SwitchSword", CombatMontage);
		break;
	default:
		break;
	}
}

void AFP_FirstPersonCharacter::Reload()
{
	IHasAmmo* AmmoRef = Cast<IHasAmmo>(WeaponComponent);
	if(AmmoRef != nullptr)
	{
		IsReloading = false;
		AmmoRef->TryReload();
		CanFire = true;
		MainPlayerController->UpdateCurrentAmmo(WeaponComponent->GetCurrentAmmo().X);
		MainPlayerController->UpdateReserveAmmo(WeaponComponent->GetCurrentAmmo().Y);
	}
}

void AFP_FirstPersonCharacter::PlayFireAnim()
{
	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();

	if(!AnimInstance)
	{
		return;
	}
	
	float AnimLegnth = 0.0f;
	float GunFireRate = 0.0f;
	float AdjustedPlayRate =1.0f;

	switch (WeaponComponent->GetWeaponInfo()->WeaponType)
	{
	case EWeaponType::TwoHand:
		AnimLegnth = CombatMontage->GetSectionLength(CombatMontage->GetSectionIndex("RifleFire"));
		GunFireRate = WeaponComponent->GetWeaponInfo()->AttackRate ;
		AdjustedPlayRate = (GunFireRate * 60.0f)/AnimLegnth;

		AnimInstance->Montage_Play(CombatMontage, AdjustedPlayRate);
		AnimInstance->Montage_JumpToSection("RifleFire", CombatMontage);
		break;
	case EWeaponType::OneHand:
		AnimLegnth = CombatMontage->GetSectionLength(CombatMontage->GetSectionIndex("PistolFire"));
		GunFireRate = WeaponComponent->GetWeaponInfo()->AttackRate;
		AdjustedPlayRate = (GunFireRate * 60.0f)/AnimLegnth;

		AnimInstance->Montage_Play(CombatMontage, AdjustedPlayRate);
		AnimInstance->Montage_JumpToSection("PistolFire", CombatMontage);
		break;
	default:
		break;
	}
	MainPlayerController->UpdateCurrentAmmo(WeaponComponent->GetCurrentAmmo().X);
	MainPlayerController->UpdateReserveAmmo(WeaponComponent->GetCurrentAmmo().Y);
}

void AFP_FirstPersonCharacter::SwordColliderOn()
{
	SwordCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AFP_FirstPersonCharacter::SwordColliderOff()
{
	SwordCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

void AFP_FirstPersonCharacter::TriggerHealthUpdate()
{
	MainPlayerController->UpdateHealth(static_cast<float>(_healthComponent->health)/_healthComponent->maxHealth);
}

void AFP_FirstPersonCharacter::TriggerPrimaryIconUpdate()
{
	if(MainPlayerController && WeaponComponent->GetWeaponInfo())
	{
		MainPlayerController->PrimaryIcon = WeaponComponent->GetWeaponInfo()->ItemIcon;
		MainPlayerController->UpdatePrimaryWeapon();
	}
}

void AFP_FirstPersonCharacter::TriggerSecondaryIconUpdate()
{
	if(MainPlayerController && GetUnusedItem())
	{
		MainPlayerController->SecondaryIcon = GetUnusedItem()->GetItemInfo()->ItemIcon;
		MainPlayerController->UpdateSecondaryWeapon();
	}
}

void AFP_FirstPersonCharacter::TriggerCrosshairToggle(bool Hidden)
{
	if(MainPlayerController)
	{
		MainPlayerController->ToggleCrosshair(Hidden);
	}
}

void AFP_FirstPersonCharacter::TriggerSniperToggle(bool Hidden)
{
	if(MainPlayerController)
	{
		MainPlayerController->ToggleSniperScope(Hidden);
	}
}

void AFP_FirstPersonCharacter::TriggerRarityUpdate()
{
	if(MainPlayerController && GetUnusedItem())
	{
		ERarity CurrentRarity = WeaponComponent->GetWeaponInfo()->ItemRarity;

		switch (CurrentRarity)
		{
		case Common:
			MainPlayerController->UpdateWeaponRarity(0);
			break;
		case Uncommon:
			MainPlayerController->UpdateWeaponRarity(1);
			break;
		case Rare:
			MainPlayerController->UpdateWeaponRarity(2);
			break;
		case SuperRare:
			MainPlayerController->UpdateWeaponRarity(3);
			break;
		case Legendary:
			MainPlayerController->UpdateWeaponRarity(4);
			break;
		default:
			break;
		}

		ERarity CurrentSecondaryRarity = GetUnusedItem()->GetItemInfo()->ItemRarity;
		switch (CurrentSecondaryRarity)
		{
		case Common:
			MainPlayerController->UpdateSecondWeaponRarity(0);
			break;
		case Uncommon:
			MainPlayerController->UpdateSecondWeaponRarity(1);
			break;
		case Rare:
			MainPlayerController->UpdateSecondWeaponRarity(2);
			break;
		case SuperRare:
			MainPlayerController->UpdateSecondWeaponRarity(3);
			break;
		case Legendary:
			MainPlayerController->UpdateSecondWeaponRarity(4);
			break;
		default:
			break;
		}
	}
}

void AFP_FirstPersonCharacter::SetHUD()
{
	TriggerHealthUpdate();
	TriggerPrimaryIconUpdate();
	TriggerSecondaryIconUpdate();
	TriggerRarityUpdate();
	TriggerSniperToggle(true);
}


void AFP_FirstPersonCharacter::MeleeDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor->IsA(AFP_FirstPersonCharacter::StaticClass()))
	{
		SwordColliderOff();
		UGameplayStatics::ApplyDamage(OtherActor, WeaponComponent->GetWeaponInfo()->Damage, GetController(), this, UDamageType::StaticClass());
		if(OtherActor->IsA(ABaseAiCharacter::StaticClass()))
		{
			ABaseAiCharacter* HitEnemy = Cast<ABaseAiCharacter>(OtherActor);
			UGameplayStatics::PlaySound2D(this, HitEnemy->DamagedSound);
		}
		
	}
}
