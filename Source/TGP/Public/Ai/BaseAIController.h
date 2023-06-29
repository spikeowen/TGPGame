// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BaseAIController.generated.h"

/**
 * 
 */
UCLASS()
class TGP_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess = true)) bool chasePlayer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess = true)) UBehaviorTree* behaviorTree;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess = true)) UBlackboardComponent* blackboardComponenet;
protected:
	void BeginPlay() override;

public:
	ABaseAIController();
	void RunBT();
	void ChangeAIControllerStatus(bool status);
	void SetObjective(AActor* objective);
};
