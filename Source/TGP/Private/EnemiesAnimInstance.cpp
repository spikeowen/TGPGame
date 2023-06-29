// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemiesAnimInstance.h"

#include "Ai/AiCharacterData.h"
#include "Ai/BaseAiCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEnemiesAnimInstance::UpdateAnimationVariables()
{
	if(!Zombie)
	{
		Zombie = Cast<ABaseAiCharacter>(TryGetPawnOwner());
	}
	else
	{
		MoveSpeed = Zombie->GetCharacterMovement()->MaxWalkSpeed;
	}
}
