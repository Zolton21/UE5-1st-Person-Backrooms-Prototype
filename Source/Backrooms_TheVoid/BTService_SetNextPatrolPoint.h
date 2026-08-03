// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_SetNextPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class BACKROOMS_THEVOID_API UBTService_SetNextPatrolPoint : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTService_SetNextPatrolPoint();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
