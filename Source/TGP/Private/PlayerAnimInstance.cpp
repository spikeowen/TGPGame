// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"

#include "Aki/AkiCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TGP/FP_FirstPerson/FP_FirstPersonCharacter.h"

void UPlayerAnimInstance::UpdateAnimationVariables()
{
	if(!Player)
	{
		Player = Cast<AFP_FirstPersonCharacter>(TryGetPawnOwner());
	}
	else
	{
		
		if(Player)
		{
			MoveSpeed = FVector(Player->GetVelocity().X, Player->GetVelocity().Y, 0.0f).Size();
			IsSprinitng = Player->IsSprinting;
			IsJumping = !Player->GetCharacterMovement()->IsMovingOnGround();
		}
	}
}
