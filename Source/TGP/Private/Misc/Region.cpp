// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/Region.h"

#include "MainPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TGP/FP_FirstPerson/FP_FirstPersonCharacter.h"

ARegion::ARegion()
{
	PrimaryActorTick.bCanEverTick = true;
	regionBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Region Bounds"));
	RootComponent = regionBounds;
}

void ARegion::BeginPlay()
{	
	Super::BeginPlay();

	regionBounds->OnComponentBeginOverlap.AddDynamic(this, &ARegion::EnteredRegion);
}

FVector ARegion::GetRandomPointInRegion()
{
	FVector boxOrigin;
	FVector boxExtent;
	float sphr; // ignore this
	UKismetSystemLibrary::GetComponentBounds(regionBounds,boxOrigin,boxExtent,sphr);
	return  UKismetMathLibrary::RandomPointInBoundingBox(boxOrigin,boxExtent);
}

void ARegion::ChangeRenderCustomDepthPass(bool state)
{
	TArray<UStaticMeshComponent*> components;
	objcetive->GetComponents<UStaticMeshComponent>(components);
	components[0]->SetRenderCustomDepth(state);
}

void ARegion::PickObjective()
{
	TArray<AActor*> possibleTargets;
	GetAttachedActors(possibleTargets,true);
	objcetive=possibleTargets[0];
	ChangeRenderCustomDepthPass(true);
}

void ARegion::EndOfRound()
{
	ChangeRenderCustomDepthPass(false);
}

void ARegion::BeginRound()
{
	PickObjective();
}

void ARegion::EnteredRegion(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(AFP_FirstPersonCharacter::StaticClass()))
	{
		AFP_FirstPersonCharacter* MainPlayer = Cast<AFP_FirstPersonCharacter>(OtherActor);
		AMainPlayerController* MainPlayerController = Cast<AMainPlayerController>(MainPlayer->GetController());
		MainPlayerController->PlayerRegion = GetName();
		MainPlayerController->UpdateCurrentZone();
	}
}

