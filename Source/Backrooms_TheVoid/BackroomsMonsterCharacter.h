// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BackroomsMonsterCharacter.generated.h"

UCLASS()
class BACKROOMS_THEVOID_API ABackroomsMonsterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABackroomsMonsterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	AActor* GetTargetActor() const;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetTarget(AActor* actor);

	UFUNCTION(BlueprintCallable)
	void VisitPatrolPoint(AActor* NewPoint);

	AActor* GetNextPatrolPoint() const;

	void FindNextPatrolPoint();
private:
	AActor* TargetActor;
	TArray<AActor*>	PatrolPoints;

	TArray<AActor*> RecentPatrolPoints;
	AActor* NextPatrolPoint;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> PatrolPointClass;
};
