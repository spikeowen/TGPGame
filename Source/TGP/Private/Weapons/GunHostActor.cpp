// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GunHostActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Weapons/WeaponComponent.h"
#include "Weapons/HitscanWeaponComponent.h"

// Sets default values
AGunHostActor::AGunHostActor()
{
	//_sceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

	SetReplicates(true);
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = _mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	_mesh->SetSimulatePhysics(true);
	_mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//SetRootComponent(_mesh);
	//_mesh->SetupAttachment(_sceneRoot);

	_weapon = CreateDefaultSubobject<UHitscanWeaponComponent>(TEXT("Weapon"));
	AddOwnedComponent(_weapon);
}

// Called when the game starts or when spawned
void AGunHostActor::BeginPlay()
{
	Super::BeginPlay();
	//_weapon->SetParentMesh(_mesh);
}

// Called every frame
void AGunHostActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGunHostActor::DebugMessage()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Screen Message"));
}

void AGunHostActor::AssignNewComponent(UWeaponComponent* component)
{
	//if(component)
	//{
	//	component->RegisterComponent();
	//	AddOwnedComponent(component);
	//}
}

