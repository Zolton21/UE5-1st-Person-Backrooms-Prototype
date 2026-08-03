// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAIController.h"
#include "Kismet/GameplayStatics.h"


void AMonsterAIController::BeginPlay() {
	Super::BeginPlay();

	if (AIBehaviour != nullptr) {
		RunBehaviorTree(AIBehaviour);

		APawn* MonsterPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}
}