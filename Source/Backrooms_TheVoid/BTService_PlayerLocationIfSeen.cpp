// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_PlayerLocationIfSeen.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAIController.h"
#include "BackroomsMonsterCharacter.h"

UBTService_PlayerLocationIfSeen::UBTService_PlayerLocationIfSeen() {
	NodeName = TEXT("Update player location if seen");
}

void UBTService_PlayerLocationIfSeen::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//TODO find nearest player
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn == nullptr) {
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr) {
		return;
	}

	ABackroomsMonsterCharacter* Monster = Cast<ABackroomsMonsterCharacter>(AIController->GetPawn());
	if (Monster == nullptr) {
		return;
	}

	if (AIController->LineOfSightTo(PlayerPawn)) {
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), PlayerPawn);
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat("TimeSinceLastSeen", 0.0f);

		Monster->SetTarget(PlayerPawn);
	}
	else {
		float TimeSinceLastSeen = OwnerComp.GetBlackboardComponent()->GetValueAsFloat("TimeSinceLastSeen");
		TimeSinceLastSeen += DeltaSeconds;
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat("TimeSinceLastSeen", TimeSinceLastSeen);

		if (TimeSinceLastSeen > 3.0f) {
			OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
			Monster->SetTarget(nullptr);
		}
	}
}