// Fill out your copyright notice in the Description page of Project Settings.


#include "BackroomsGameMode.h"
#include "BackroomsPlayerCharacter.h"
#include "EngineUtils.h"

void ABackroomsGameMode::PlayerKilled(APawn* KilledPawn) {
	EndGame(false);
}

void ABackroomsGameMode::PlayerReachedExit() {
	EndGame(true);
}

void ABackroomsGameMode::EndGame(bool bIsPlayerWinner) {
	for (AController* Controller : TActorRange<AController>(GetWorld())) {
		bool bIsWinner = Controller->IsPlayerController() == bIsPlayerWinner;
		Controller->GameHasEnded(Controller->GetPawn(), bIsWinner);
	}
}