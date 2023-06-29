// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/HealthComponent.h"

#include "MainPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TGP/FP_FirstPerson/FP_FirstPersonCharacter.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// ...

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::ApplyDamage);
	health = maxHealth;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Linked OnTakeAnyDamage"));
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UHealthComponent::AdjustHealth(AController* causer, float damage)
{
	health -= damage;
	onComponentTakeDamage.Broadcast(causer, damage);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("New Health: ") + FString::FromInt(health));
	if(health <= 0)
	{
		dead = true;
		onComponentDead.Broadcast(causer);
		if(destroyOnDeath)
			KillObject();
	}
	else if(health > maxHealth)
	{
		health = maxHealth;
	}
	return dead;
}

void UHealthComponent::KillObject()
{
	GetOwner()->Destroy();
}


void UHealthComponent::ApplyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser )
{
	AdjustHealth(InstigatedBy, Damage);

	if(DamagedActor->IsA(AFP_FirstPersonCharacter::StaticClass()))
	{
		AFP_FirstPersonCharacter* MainPlayer = Cast<AFP_FirstPersonCharacter>(DamagedActor);
		MainPlayer->TriggerHealthUpdate();
	}
}

