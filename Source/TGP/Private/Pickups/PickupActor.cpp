// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/PickupActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APickupActor::APickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(_root);

	_staticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	_staticMesh->SetupAttachment(_root);
	_staticMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();
	_staticMesh->OnComponentBeginOverlap.AddDynamic(this, &APickupActor::OnOverlapWithActor);
}

void APickupActor::OnOverlapWithActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OVERLAP EVENT: Pickup Comp"));
	
}

// Called every frame
void APickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	_t += DeltaTime;

	FVector loc = GetActorLocation();
	loc += FVector(0.0f, 0.0f, FMath::Sin(_t * 1.5f) * 20.0f * DeltaTime);
	SetActorLocation(loc);

	FRotator rot = GetActorRotation();
	rot += FRotator(0.0f, 180.0f * DeltaTime, 0.0f);
	SetActorRotation(rot);
}

