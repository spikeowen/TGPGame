// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/Interfaces/WeaponInterfaces.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FComponentDeadSignature, AController*, causer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FComponentTookDamageSignature, AController*, causer, float, damage);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TGP_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable, Category = "Components") FComponentDeadSignature onComponentDead;
	UPROPERTY(BlueprintAssignable, Category = "Components") FComponentTookDamageSignature onComponentTakeDamage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int maxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool destroyOnDeath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int health;
	bool dead;
	
	void SetHealth(float newHealth) { health = newHealth; dead = false; }
	bool AdjustHealth(AController* causer, float damage);
	bool isDead() { return dead; }
	virtual void KillObject();
	
	UFUNCTION() void ApplyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser );
	
};
