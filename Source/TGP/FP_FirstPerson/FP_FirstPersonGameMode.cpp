// Copyright Epic Games, Inc. All Rights Reserved.

#include "FP_FirstPersonGameMode.h"
#include "FP_FirstPersonHUD.h"
#include "FP_FirstPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFP_FirstPersonGameMode::AFP_FirstPersonGameMode()
{
	HUDClass = AFP_FirstPersonHUD::StaticClass();
}