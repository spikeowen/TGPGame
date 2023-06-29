// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"

#include "Region.generated.h"

UCLASS()
class TGP_API ARegion : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess = true))
	UBoxComponent* regionBounds;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess = true))
	FString name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess = true))
	AActor* objcetive;

	virtual void BeginPlay() override;

	void ChangeRenderCustomDepthPass(bool state);
public:	
	ARegion();
	FVector GetRandomPointInRegion();
	FString GetName(){return name;}
	AActor* GetObjective(){return objcetive;}
	void PickObjective();
	void EndOfRound();
	void BeginRound();


	UFUNCTION()
	void EnteredRegion(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
};
