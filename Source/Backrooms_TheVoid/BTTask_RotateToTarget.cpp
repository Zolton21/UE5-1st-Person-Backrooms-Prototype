// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RotateToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_RotateToTarget::UBTTask_RotateToTarget() {
	NodeName = TEXT("RotateToTarget");
	bNotifyTick = true;
	//AcceptableAngle = 5.0f;
	//RotationSpeed = 5.0f;
}

EBTNodeResult::Type UBTTask_RotateToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!BBComp)
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(TargetKey.SelectedKeyName));
	if (TargetActor)
	{
		AIController->SetFocus(TargetActor); // AI will rotate smoothly toward the actor
		return EBTNodeResult::Succeeded;
	}

	// Optional: clear focus if no target
	AIController->ClearFocus(EAIFocusPriority::Gameplay);
	return EBTNodeResult::Failed;
}

/*void UBTTask_RotateToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Fail 1"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	APawn* Pawn = AIController->GetPawn();
	if (Pawn == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Fail 2"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UBlackboardComponent* BBComponent = OwnerComp.GetBlackboardComponent();
	if (BBComponent == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Fail 3"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FVector TargetLocation = BBComponent->GetValueAsVector(TargetKey.SelectedKeyName);
	FVector PawnLocation = Pawn->GetActorLocation();

	FVector Direction = TargetLocation - PawnLocation;
	Direction.Z = 0.0f;
	if (Direction.IsNearlyZero()) {
		UE_LOG(LogTemp, Warning, TEXT("Success 1"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	FRotator CurrentRotation = Pawn->GetActorRotation();
	FRotator TargetRot = Direction.Rotation();

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRot, DeltaSeconds, RotationSpeed);
	Pawn->SetActorRotation(NewRotation);

	float AngleDiff = FMath::Abs(FRotator::NormalizeAxis(NewRotation.Yaw - TargetRot.Yaw));
	if (AngleDiff < AcceptableAngle) {
		UE_LOG(LogTemp, Warning, TEXT("Success 2"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("None"));

}*/