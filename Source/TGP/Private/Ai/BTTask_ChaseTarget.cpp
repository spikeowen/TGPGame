// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/BTTask_ChaseTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ChaseTarget::UBTTask_ChaseTarget()
{
	NodeName = TEXT("Chase Target");
}

EBTNodeResult::Type UBTTask_ChaseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AActor* targetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));
	
	OwnerComp.GetAIOwner()->MoveToActor(targetActor,50.0f,true,true,true,0,true);
	
	return EBTNodeResult::Succeeded;
}