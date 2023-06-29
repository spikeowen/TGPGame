// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/BTTask_DistanceCheck.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_DistanceCheck::UBTTask_DistanceCheck()
{
	NodeName = TEXT("Distance Check");
}

EBTNodeResult::Type UBTTask_DistanceCheck::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* blackboardComponent = OwnerComp.GetBlackboardComponent();
	const FVector targetLocation = Cast<AActor>(blackboardComponent->GetValueAsObject(GetSelectedBlackboardKey()))->GetActorLocation();
	const FVector actorLocaton = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();
	blackboardComponent->SetValueAsBool("inAttackRange",FVector::Distance(actorLocaton,targetLocation)<200.0f);
	
	return EBTNodeResult::Succeeded;
}