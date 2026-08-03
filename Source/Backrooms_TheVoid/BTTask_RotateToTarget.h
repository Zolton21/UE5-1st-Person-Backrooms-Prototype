// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RotateToTarget.generated.h"

/**
 * 
 */
UCLASS()
class BACKROOMS_THEVOID_API UBTTask_RotateToTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_RotateToTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetKey;

    /*
    UPROPERTY(EditAnywhere, Category = "Rotate")
    float AcceptableAngle;

    UPROPERTY(EditAnywhere, Category = "Rotate")
    float RotationSpeed;
    */
};
