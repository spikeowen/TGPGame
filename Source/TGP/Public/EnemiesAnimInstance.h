// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemiesAnimInstance.generated.h"

/**
 * 
 */
UENUM()
enum class MovingState
{
	EMS_SlowWalk	UMETA(DisplayName = "SlowWalk"),
	EMS_Walk		UMETA(DisplayName = "Walk"),
	EMS_Run			UMETA(DisplayName = "Run")
};

UCLASS()
class TGP_API UEnemiesAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationVariables();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	float MoveSpeed; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	MovingState MoveState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy")
	class ABaseAiCharacter* Zombie;
	
};
