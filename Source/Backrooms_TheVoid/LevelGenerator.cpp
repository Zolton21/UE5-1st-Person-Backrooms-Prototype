// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGenerator.h"

// Sets default values
ALevelGenerator::ALevelGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();

	/*if (seed == 0) {
		seed = FDateTime::Now().GetMillisecond() + FDateTime::Now().GetSecond() * 1000;
	}*/

	RandStream = FRandomStream(seed);
	FIntPoint StartCoord(0, 0);
	PlacedChunks.Add(StartCoord);
	TSet<FIntPoint> PotentialChunks;
	TArray<int32> AllShuffledValues;
	TArray<int32> ChunksWithoutConnectionPriority;
	TSet<FIntPoint> ChunksWithFourConnections;
	FIntPoint LevelStart;
	FIntPoint LevelExit;

	AllShuffledValues.Empty();
	AllShuffledValues = ArrayOfRandomValuesInRange(2, MaxChunks);
	for (int32 i = 0; i < MaxChunks / 7; ++i) {
		ChunksWithoutConnectionPriority.Add(AllShuffledValues[i]);
	}

	while (PlacedChunks.Num() < MaxChunks) {
		PotentialChunks.Empty();

		for (FIntPoint ChunkCoord : PlacedChunks) {
			for (const FIntPoint& Dir : Directions) {
				FIntPoint PotentialChunkCoord = ChunkCoord + Dir;
				if (!PlacedChunks.Contains(PotentialChunkCoord)) {
					if(ChunksWithoutConnectionPriority.Contains(PlacedChunks.Num())){
						if (GetConnectionsCountForTheChunk(PotentialChunkCoord) == 1) {
							PotentialChunks.Add(PotentialChunkCoord);
						}
					}
					else {
						if (GetConnectionsCountForTheChunk(PotentialChunkCoord) >= 2) {
							PotentialChunks.Add(PotentialChunkCoord);
						}
					}
				}
			}
		}

		FIntPoint NextChunkCoord;

		if (PotentialChunks.IsEmpty()) {
			int32 maxIndex = PlacedChunks.GetMaxIndex();
			TArray<FIntPoint> TempArray = PlacedChunks.Array();
			int32 RandIndex = RandStream.RandRange(0, TempArray.Num() - 1);
			FIntPoint selectedChunk = TempArray[RandIndex];
			
			for (const FIntPoint& Dir : Directions) {
				if (!PlacedChunks.Contains(selectedChunk + Dir)) {
					NextChunkCoord = selectedChunk + Dir;
					break;
				}
			}
		}
		else {
			int32 maxIndex = PotentialChunks.GetMaxIndex();
			TArray<FIntPoint> TempArray = PotentialChunks.Array();
			int32 RandIndex = RandStream.RandRange(0, TempArray.Num() - 1);
			NextChunkCoord = TempArray[RandIndex];
		}
		PlacedChunks.Add(NextChunkCoord);
	}
	for (const FIntPoint& Chunk : PlacedChunks) {
		if (GetConnectionsCountForTheChunk(Chunk) == 4)
			ChunksWithFourConnections.Add(Chunk);
	}

	AllShuffledValues.Empty();
	AllShuffledValues = ArrayOfRandomValuesInRange(0, ChunksWithFourConnections.Num());

	//Spawn floor
	for (const FIntPoint& Chunk : PlacedChunks) {
		FVector SpawnLocation = FVector(Chunk.X * ChunkSpacing, Chunk.Y * ChunkSpacing, 0);
		if (FloorChunkClass) {
			GetWorld()->SpawnActor<AActor>(FloorChunkClass, SpawnLocation, FRotator::ZeroRotator);
		}
	}
	//Spawn walls
	for (const FIntPoint& Chunk : PlacedChunks) {
		for (const FIntPoint& Dir : Directions) {
			if (!PlacedChunks.Contains(Chunk + Dir)) {
				FVector SpawnLocation = FVector(Chunk.X * ChunkSpacing, Chunk.Y * ChunkSpacing, 0);
				FRotator Rotator(0.f, GetWallRotation(Dir), 0.f);
				if (WallChunkClass) {
					GetWorld()->SpawnActor<AActor>(WallChunkClass, SpawnLocation, Rotator);
				}
			}
		}
	}
	//Spawn Start and Exit
	float MaxDistanceSq = 0.0f;

	TArray<FIntPoint> NonEmptyChunksArray = PlacedChunks.Array(); // Convert TSet to TArray for indexed access

	FIntPoint FurthestA = NonEmptyChunksArray[0];
	FIntPoint FurthestB = NonEmptyChunksArray[1];

	for (int32 i = 0; i < NonEmptyChunksArray.Num(); ++i)
	{
		for (int32 j = i + 1; j < NonEmptyChunksArray.Num(); ++j)
		{
			float DistanceSq = FVector2D::DistSquared(FVector2D(NonEmptyChunksArray[i]), FVector2D(NonEmptyChunksArray[j]));
			if (DistanceSq > MaxDistanceSq)
			{
				MaxDistanceSq = DistanceSq;
				FurthestA = NonEmptyChunksArray[i];
				FurthestB = NonEmptyChunksArray[j];
			}
		}
	}
	LevelStart = FurthestA;
	LevelExit = FurthestB;

	FVector StartSpawnLocation = FVector(LevelStart.X * ChunkSpacing, LevelStart.Y * ChunkSpacing, 0);
	FVector EndSpawnLocation = FVector(LevelExit.X * ChunkSpacing, LevelExit.Y * ChunkSpacing, 0);
	FVector DirectionAToB = EndSpawnLocation - StartSpawnLocation;
	FVector DirectionBToA = StartSpawnLocation - EndSpawnLocation;
	float YawA = FRotationMatrix::MakeFromX(DirectionAToB).Rotator().Yaw;
	float YawB = FRotationMatrix::MakeFromX(DirectionBToA).Rotator().Yaw;
	YawA = FMath::RoundToFloat(YawA / 90.0f) * 90.0f;
	YawB = FMath::RoundToFloat(YawB / 90.0f) * 90.0f;

	if (LevelStartClass) {
		GetWorld()->SpawnActor<AActor>(LevelStartClass, StartSpawnLocation, FRotator(0.f, YawA, 0.f));
	}

	if (LevelExitClass) {
		GetWorld()->SpawnActor<AActor>(LevelExitClass, EndSpawnLocation, FRotator(0.f, YawB, 0.f));
	}
}

// Called every frame
void ALevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TArray<int32> ALevelGenerator::ArrayOfRandomValuesInRange(int a, int b) const {
	TArray<int32> AllValues;
	for (int32 i = a; i < b; ++i) {
		AllValues.Add(i);
	}
	for (int32 i = AllValues.Num() - 1; i > 0; --i) {
		int32 j = RandStream.RandRange(0, i);
		AllValues.Swap(i, j);
	}
	return AllValues;
}

int32 ALevelGenerator::GetConnectionsCountForTheChunk(FIntPoint NextChunkCoord) const{
	int32 ChunkConnections = 0;
	for (const FIntPoint& Dir : Directions) {
		if (PlacedChunks.Contains(NextChunkCoord + Dir)) {
			ChunkConnections++;
		}
	}
	return ChunkConnections;
}

double ALevelGenerator::GetWallRotation(FIntPoint dir) const {
	if (dir == FIntPoint(1, 0))
		return 90.f;
	if (dir == FIntPoint(-1, 0))
		return -90.f;
	if (dir == FIntPoint(0, 1))
		return 180.f;

	return 0.f;
}

FIntPoint ALevelGenerator::FindMinMaxOfArray(TArray<int32> arr) const {
	int32 Min = arr[0];
	int32 Max = arr[0];

	for (int32 Value : arr)
	{
		if (Value < Min) Min = Value;
		if (Value > Max) Max = Value;
	}
	return FIntPoint(Min, Max);
}

TArray<TPair<FIntPoint, FIntPoint>> ALevelGenerator::GetFurthestChunkPairs(const TSet<FIntPoint>&Chunks, int32 amount) {
	TArray<FIntPoint> ChunkArray = Chunks.Array();
	TArray<TTuple<float, FIntPoint, FIntPoint>> Distances;

	const int32 Count = ChunkArray.Num();

	for (int32 i = 0; i < Count; ++i)
	{
		for (int32 j = i + 1; j < Count; ++j)
		{
			const FIntPoint& A = ChunkArray[i];
			const FIntPoint& B = ChunkArray[j];

			float Distance = FVector2D::Distance(FVector2D(A), FVector2D(B));
			Distances.Add(MakeTuple(Distance, A, B));
		}
	}

	// Sort by distance descending
	Distances.Sort([](const TTuple<float, FIntPoint, FIntPoint>& A, const TTuple<float, FIntPoint, FIntPoint>& B)
		{
			return A.Get<0>() > B.Get<0>();
		});

	// Extract top <amount> pairs
	TArray<TPair<FIntPoint, FIntPoint>> Result;
	const int32 MaxPairs = FMath::Min(amount, Distances.Num());

	for (int32 i = 0; i < MaxPairs; ++i)
	{
		Result.Add(TPair<FIntPoint, FIntPoint>(Distances[i].Get<1>(), Distances[i].Get<2>()));
	}

	return Result;
}