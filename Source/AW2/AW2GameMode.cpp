// Copyright Epic Games, Inc. All Rights Reserved.

#include "AW2GameMode.h"
#include "AW2Character.h"
#include "UObject/ConstructorHelpers.h"

AAW2GameMode::AAW2GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
