// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AiCharacterData.generated.h"

UCLASS()
class TGP_API UAiCharacterData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats") float DefaultHealth = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats") float DefaultDamage = 15.f;

	// Used for randomizing the spawn stats the enemy will spawn with. Example: 0.2f deviation = enemies can spawn with health ranging from 80 - 120. Same for other stats like Damage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta=(ClampMax = "1.0", ClampMin = "0.0")) float MaxDeviation = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement") float DefaultWalkSpeed = 100.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals") TArray<USkeletalMesh*> SkeletalMeshes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals") TSubclassOf<UAnimInstance> EnemyAnimBP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ai") TSubclassOf<AController> AiControllerClass;
};
