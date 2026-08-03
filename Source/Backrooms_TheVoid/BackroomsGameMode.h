// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BackroomsGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BACKROOMS_THEVOID_API ABackroomsGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void PlayerReachedExit();

	UFUNCTION(BlueprintCallable)
	void PlayerKilled(APawn* KilledPawn);

	void EndGame(bool bIsPlayerWinner);
};
