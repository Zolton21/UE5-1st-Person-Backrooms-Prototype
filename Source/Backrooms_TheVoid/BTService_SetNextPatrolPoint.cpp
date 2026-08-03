// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetNextPatrolPoint.h"
#include "BackroomsMonsterCharacter.h"
#include "MonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTService_SetNextPatrolPoint::UBTService_SetNextPatrolPoint() {
	NodeName = TEXT("Set next patrol point");
}

void UBTService_SetNextPatrolPoint::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr) {
		return;
	}

	ABackroomsMonsterCharacter* Monster = Cast<ABackroomsMonsterCharacter>(AIController->GetPawn());
	if (Monster == nullptr) {
		return;
	}

	AActor* TargetedPatrolPoint = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));
	AActor* NextPatrolPoint = Monster->GetNextPatrolPoint();

	if (TargetedPatrolPoint != NextPatrolPoint) {
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), NextPatrolPoint->GetActorLocation());
	}
}