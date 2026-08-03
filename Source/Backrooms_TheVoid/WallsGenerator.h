// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "WallsGenerator.generated.h"

struct FWallKey {
	FVector Location;
	float Rotation;

	bool operator==(const FWallKey& Other) const {
		return Location == Other.Location && FMath::IsNearlyEqual(Rotation, Other.Rotation, 0.01f);
	}
};

FORCEINLINE uint32 GetTypeHash(const FWallKey& Key) {
	return HashCombine(GetTypeHash(Key.Location), GetTypeHash(Key.Rotation));
}

UCLASS()
class BACKROOMS_THEVOID_API AWallsGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWallsGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	struct FWallGrowthPoint {
		FVector Position;
		FVector Direction;
		int32 Age;
		FVector BranchDirection;
		int32 BranchesCreated;
		int32 CollisionAvoidanceValueLength;
		int32 CollisionAvoidanceValueWidth;
		FWallGrowthPoint(FVector InPos, FVector InDir, int32 InAge, FVector InBranchDirection, int32 InBranchesCreated,
			int32 InCollisionAvoidanceValueLength, int32 InCollisionAvoidanceValueWidth)
			: Position(InPos), Direction(InDir), Age(InAge), BranchDirection(InBranchDirection), BranchesCreated(InBranchesCreated),
			CollisionAvoidanceValueLength(InCollisionAvoidanceValueLength), CollisionAvoidanceValueWidth(InCollisionAvoidanceValueWidth){
		}

		bool operator==(const FWallGrowthPoint& Other) const{
			return Position == Other.Position && Direction == Other.Direction;
		}
	};

	struct FAreaPair {
		FVector Area1Center;
		FVector Area2Center;

		FAreaPair(FVector InA, FVector InB)
			: Area1Center(InA), Area2Center(InB) {
		}

		FAreaPair() = default;
	};

	//TMap<FWallKey, AActor*> PlacedWalls;
	TSet<FWallKey> PlacedWalls;

	TSet<FVector> OccupiedPositions;
	
	void PlaceWall(FVector Location, FVector Direction);
	void SpawnWalls();
	void RemoveRecentWalls(const FWallGrowthPoint& GrowthPoint);
	double GetWallRotation(FVector dir) const;
	void GenerateWalls();
	TPair<FVector, FVector> GetPerpendicularDirections(FVector dir) const;
	bool IsNearExistingWall(const FWallGrowthPoint& GrowthPoint) const;
	double GetBranchProbability(int32 age) const;
	bool IsAtTheEdge(const FWallGrowthPoint& GrowthPoint) const;
	bool CanGenerateRandomWall(const FWallGrowthPoint& GrowthPoint) const;
	void GenerateBorder();
	bool CanBranch(const FWallGrowthPoint& GrowthPoint) const;
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SpawnStart();
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SpawnExit();
	void SpawnColumns();
	float SnapToGrid(float Value, float GridSize = 100.0f);

	const int32 CyclesPerTick = 500;
	const float MaxDelay = 0.1f;
	int32 InnerCycleIndex = 0;

	const float WallLength = 100.0f;

	const TArray<FVector> Directions = {
		FVector(WallLength, 0, 0),	//E
		FVector(-WallLength, 0, 0),	//W
		FVector(0, WallLength, 0),	//N
		FVector(0, -WallLength, 0)	//S
	};
	TArray<FWallGrowthPoint> ActiveGrowthPoint;
	FTimerHandle TimeHandle;
	FRandomStream RandStream;
	double EdgeCoordingate = 20000.0f;
	int32 RandomWallLength = 3; //3 * 100(Free space) -> 3 * 100(Walls) -> 3 * 100(Free space)

	UPROPERTY(EditAnywhere, Category = "Generation")
	TSubclassOf<AActor> WallClass;

	UPROPERTY(EditAnywhere, Category = "Generation")
	TSubclassOf<AActor> ColumnsGeneratorClass;

	UPROPERTY(EditAnywhere, Category = "Generation")
	TSubclassOf<AActor> LevelStartClass;

	UPROPERTY(EditAnywhere, Category = "Generation")
	TSubclassOf<AActor> LevelExitClass;

	UPROPERTY(EditAnywhere, Category = "Generation")
	TSubclassOf<AActor> PathPointClass;

	UPROPERTY(EditAnywhere, Category = "Generation")
	float delay = 0.05f;

	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetColumnSpawnChance();

	UFUNCTION(BlueprintCallable, Category = "Generation")
	int32 GetWorldSeed();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (ExposeOnSpawn = true))
	int32 MinAgeToAllowBranchSpawn = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (ExposeOnSpawn = true))
	int32 AgeToForceBranchSpawn = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (ExposeOnSpawn = true))
	int32 CollisionAvoidanceValueMin = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (ExposeOnSpawn = true))
	int32 CollisionAvoidanceValueMax = 9;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (ExposeOnSpawn = true))
	int32 WorldSeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (ExposeOnSpawn = true))
	float RandomWallSpawnChance = 0.05;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (ExposeOnSpawn = true))
	float ColumnSpawnChance = 0.1;
};