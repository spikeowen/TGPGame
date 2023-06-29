// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/BTTask_RandomWalk.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_RandomWalk::UBTTask_RandomWalk()
{
	NodeName = TEXT("Random Walk");
}

EBTNodeResult::Type UBTTask_RandomWalk::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const FVector actorLocaton = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();
	FVector targetLocation = UNavigationSystemV1::GetRandomPointInNavigableRadius(GetWorld(),actorLocaton,500);

	OwnerComp.GetBlackboardComponent()->SetValueAsVector("wanderLocation",targetLocation);

	return EBTNodeResult::Succeeded;
}
