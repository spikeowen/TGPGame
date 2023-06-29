// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyDamageMarker.generated.h"

UCLASS()
class TGP_API AMyDamageMarker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyDamageMarker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void WidgetBillboardAndMove(float DeltaTime);

	UPROPERTY() class USceneComponent* _root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) class UWidgetComponent* _widget;

	UPROPERTY() AActor* _spawnedBy;
	UPROPERTY() APlayerController* _controller;
	float _aliveTime;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable) void SetText(int num);
	UFUNCTION(BlueprintCallable) void SetSpawnedBy(AActor* spawned);
	UWidgetComponent* GetWidget() { return _widget; }
};

