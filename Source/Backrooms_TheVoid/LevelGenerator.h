// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"

UCLASS()
class BACKROOMS_THEVOID_API ALevelGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	int32 GetConnectionsCountForTheChunk(FIntPoint NextChunkCoord) const;
	TArray<int32> ArrayOfRandomValuesInRange(int a, int b) const;
	double GetWallRotation(FIntPoint dir) const;
	FIntPoint FindMinMaxOfArray(TArray<int32> arr) const;
	TArray<TPair<FIntPoint, FIntPoint>> GetFurthestChunkPairs(const TSet<FIntPoint>& Chunks, int32 amount);

	const TArray<FIntPoint> Directions = {
		FIntPoint(1, 0),	//E
		FIntPoint(-1, 0),	//W
		FIntPoint(0, 1),	//N
		FIntPoint(0, -1)	//S
	};

	TSet<FIntPoint> PlacedChunks;
	FRandomStream RandStream;

	UPROPERTY(EditAnywhere, Category = "Generation")
	int32 seed;

	UPROPERTY(EditAnywhere, Category = "Generation")
	int32 MaxChunks = 500;

	UPROPERTY(EditAnywhere, Category = "Generation")
	float ChunkSpacing = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Generation")
	TSubclassOf<AActor> FloorChunkClass;

	UPROPERTY(EditAnywhere, Category = "Generation")
	TSubclassOf<AActor> WallChunkClass;

	UPROPERTY(EditAnywhere, Category = "Generation")
	TSubclassOf<AActor> LevelStartClass;

	UPROPERTY(EditAnywhere, Category = "Generation")
	TSubclassOf<AActor> LevelExitClass;
};
