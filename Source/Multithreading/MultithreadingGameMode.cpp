// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultithreadingGameMode.h"
#include "MultithreadingCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMultithreadingGameMode::AMultithreadingGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
