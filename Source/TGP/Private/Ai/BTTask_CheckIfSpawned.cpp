// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/BTTask_CheckIfSpawned.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_CheckIfSpawned::UBTTask_CheckIfSpawned()
{
	NodeName = TEXT("Check If Spawned");
}

EBTNodeResult::Type UBTTask_CheckIfSpawned::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	return OwnerComp.GetBlackboardComponent()->GetValueAsBool(("isSpawned"))
	? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}