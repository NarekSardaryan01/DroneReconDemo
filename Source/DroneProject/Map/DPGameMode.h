// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DPGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DRONEPROJECT_API ADPGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:


	ADPGameMode();

	void BeginPlay() override;
	
};
