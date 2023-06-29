#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapons/HealthComponent.h"
#include "TGP/Public/AI/BaseAIController.h"

#include "BaseAiCharacter.generated.h"

class UAiCharacterData;

UENUM(BlueprintType)
enum class EZombieMoveType : uint8
{
	MoveState_SlowWalking		UMETA(DisplayName = "Slow Walking"),
	MoveState_NormalWalking	UMETA(DisplayName = "Normal Walking"),
	MoveState_Running			UMETA(DisplayName = "Running")
};

UCLASS()
class TGP_API ABaseAiCharacter : public ACharacter
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess = true)) UAiCharacterData* EnemyStats;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess = true)) UHealthComponent* HealthComponent;
	
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", Meta = (AllowPrivateAccess = true)) EZombieMoveType ZombieType;
	float RunMultiplier = 3.0f;
	float SlowWalkMultiplier = 0.5f;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations") class UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animations") class UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Attack") class USphereComponent* RightHandCollider;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Attack") class UCapsuleComponent* LeftArmCollider;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Audio") class USoundCue* DamagedSound;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY() ABaseAIController* baseAiController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Audio") class USoundCue* AttackSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Audio") class USoundCue* HeavyAttackSound;
	
	bool StopRepeatAnim;
	
public:
	ABaseAiCharacter();
	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	bool IsAlive() const { return HealthComponent->health > 0.f; }

	UFUNCTION() void OnEnemyDied(AController* Causer);
	void SpawnEnemy(const FVector& RespawnPos);

	void SetHidden(bool bEnemyHidden);

	void Attack();

	void SetMoveType();

	UFUNCTION(BlueprintCallable)
	void RightColliderOn();
	UFUNCTION(BlueprintCallable)
	void RightColliderOff();
	UFUNCTION()
	void RightColliderHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION(BlueprintCallable)
	void LeftColliderOn();
	UFUNCTION(BlueprintCallable)
	void LeftColliderOff();
	UFUNCTION()
	void LeftColliderHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION(BlueprintCallable)
	void Die();
	
	UPROPERTY(VisibleAnywhere) bool bIsDead = true;
};
