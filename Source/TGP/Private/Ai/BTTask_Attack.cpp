// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/BTTask_Attack.h"
#include "TGP/Public/Ai/BaseAiCharacter.h"


UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	Cast<ABaseAiCharacter>(OwnerComp.GetAIOwner()->GetPawn())->Attack();
	
	return EBTNodeResult::Succeeded;
}
