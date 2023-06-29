// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/UI/WeaponStatUIActor.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SceneComponent.h"
#include "Weapons/UI/WeaponStatUIWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Item/BaseItem.h"
#include "Item/ItemActor.h"
#include "Item/ItemInfo.h"


// Sets default values
AWeaponStatUIActor::AWeaponStatUIActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(_root);
	
	_widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	_widget->SetupAttachment(_root);
}

// Called when the game starts or when spawned
void AWeaponStatUIActor::BeginPlay()
{
	Super::BeginPlay();

	_playerController = nullptr;
}

// Called every frame
void AWeaponStatUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void AWeaponStatUIActor::SetWeaponStats(UWeaponInfo* info)
{
	UWidgetComponent* widget = GetWidget();
	UUserWidget* userWid = widget->GetUserWidgetObject();
	UWeaponStatUIWidget* castedWidget = Cast<UWeaponStatUIWidget>(userWid);
	if(castedWidget)
	{
		castedWidget->SetText("Test");
	}
}

