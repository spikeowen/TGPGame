// Fill out your copyright notice in the Description page of Project Settings.

#include "Ai/BaseAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ABaseAIController::ABaseAIController()
{
	blackboardComponenet = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	AddOwnedComponent(blackboardComponenet);
}

void ABaseAIController::ChangeAIControllerStatus(bool status)
{
	if(blackboardComponenet)
		blackboardComponenet->SetValueAsBool("isSpawned", status);
	
	if (status)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Green, FString::Printf(TEXT("Enemy Spawned")));
	}
}

void ABaseAIController::SetObjective(AActor* objective)
{
	if(chasePlayer)
	{
		ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
		blackboardComponenet->SetValueAsObject("target",player);
	}
	else
	{
		blackboardComponenet->SetValueAsObject("objective",objective);
	}
	
}

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseAIController::RunBT()
{
	RunBehaviorTree(behaviorTree);
}