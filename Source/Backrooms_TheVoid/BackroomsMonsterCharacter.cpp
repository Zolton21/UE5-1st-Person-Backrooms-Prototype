// Fill out your copyright notice in the Description page of Project Settings.


#include "BackroomsMonsterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "BackroomsPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

// Sets default values
ABackroomsMonsterCharacter::ABackroomsMonsterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;  // rotates with velocity
	bUseControllerRotationYaw = false;
}

// Called when the game starts or when spawned
void ABackroomsMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (PatrolPointClass) {
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), PatrolPointClass, PatrolPoints);

		FindNextPatrolPoint();
	}
}

void ABackroomsMonsterCharacter::FindNextPatrolPoint() {
	TArray<AActor*> NextPatrolPointCandidates;
	
	for (AActor* Point : PatrolPoints) {
		if (!RecentPatrolPoints.Contains(Point)) {
			NextPatrolPointCandidates.Add(Point);
		}
	}

	if (NextPatrolPointCandidates.Num() == 0) {
		return;
	}

	FVector MonsterLocation = GetActorLocation();
	TArray<AActor*> Sorted = NextPatrolPointCandidates;
	Sorted.Sort([&](const AActor& A, const AActor& B) {
		return FVector::DistSquared(MonsterLocation, A.GetActorLocation()) < FVector::DistSquared(MonsterLocation, B.GetActorLocation());
		});

	int32 MaxCount = FMath::Min(3, Sorted.Num());
	TArray<AActor*> Closest;
	for (int32 i = 0; i < MaxCount; i++)
	{
		Closest.Add(Sorted[i]);
	}

	NextPatrolPoint = Closest[FMath::RandRange(0, Closest.Num() - 1)];
	FVector Location = NextPatrolPoint->GetActorLocation();
}

void ABackroomsMonsterCharacter::VisitPatrolPoint(AActor* NewPoint) {
	if (NewPoint == nullptr) {
		return;
	}

	RecentPatrolPoints.Add(NewPoint);

	if (RecentPatrolPoints.Num() > FMath::Min(3, PatrolPoints.Num() - 1)) {
		RecentPatrolPoints.RemoveAt(0);
	}

	FindNextPatrolPoint();
}

// Called every frame
void ABackroomsMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABackroomsMonsterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABackroomsMonsterCharacter::SetTarget(AActor* actor) {
	TargetActor = actor;
	if (actor != nullptr) {
		// Start patrolling
		if (AAIController* AICon = Cast<AAIController>(GetController())) {
			AICon->ClearFocus(EAIFocusPriority::Gameplay);
		}
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bUseControllerRotationYaw = false;
	}
	else {
		// Start chasing
		if (AAIController* AICon = Cast<AAIController>(GetController())) {
			AICon->SetFocus(actor, EAIFocusPriority::Gameplay);
		}
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
	}
}

AActor* ABackroomsMonsterCharacter::GetTargetActor() const {
	return TargetActor;
}

AActor* ABackroomsMonsterCharacter::GetNextPatrolPoint() const {
	return NextPatrolPoint;
}
