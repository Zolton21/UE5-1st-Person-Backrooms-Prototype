// Fill out your copyright notice in the Description page of Project Settings.


#include "WallsGenerator.h"

// Sets default values
AWallsGenerator::AWallsGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWallsGenerator::BeginPlay()
{
	Super::BeginPlay();

	RandStream = FRandomStream(WorldSeed);
	int32 NumSeeds = RandStream.RandRange(5, 8);

	int32 addend = 2 * PI / NumSeeds;
	for (int32 i = 0; i < NumSeeds; i++) {
		float angle = RandStream.FRandRange(i * addend, (i + 1) * addend);
		float radius = EdgeCoordingate * RandStream.FRandRange(0.3f, 0.7f);
		float x = radius * FMath::Cos(angle);
		float y = radius * FMath::Sin(angle);

		FVector Location(SnapToGrid(x, WallLength), SnapToGrid(y, WallLength), 0);
		FVector Direction = Directions[RandStream.RandRange(0, Directions.Num() - 1)];

		TPair<FVector, FVector> PerpendicularVectors = GetPerpendicularDirections(Direction);
		FVector BranchDir = RandStream.RandRange(0, 1) == 0 ? PerpendicularVectors.Key : PerpendicularVectors.Value;
		//Testing
		ActiveGrowthPoint.Add(FWallGrowthPoint(Location, Direction, 0, BranchDir, 0, RandStream.RandRange(CollisionAvoidanceValueMin, CollisionAvoidanceValueMax), RandStream.RandRange(CollisionAvoidanceValueMin, CollisionAvoidanceValueMax)));
		if (i == 0) {
			if (PathPointClass) {
				GetWorld()->SpawnActor<AActor>(PathPointClass, FVector(Location - Direction), FRotator(0, 0, 0));
			}
		}
	}	
	GenerateWalls();
}

void AWallsGenerator::GenerateWalls() {
	if(ActiveGrowthPoint.IsEmpty()) {
		GenerateBorder();
	}else {
		
		TArray<FWallGrowthPoint> PointsToDelete;
		TArray<FWallGrowthPoint> RandomWallPoints;
		for (int32 j = 0; j < ActiveGrowthPoint.Num(); ++j) {
			if (IsNearExistingWall(ActiveGrowthPoint[j]) || IsAtTheEdge(ActiveGrowthPoint[j])) {
				if (ActiveGrowthPoint[j].Age < 5) {
					RemoveRecentWalls(ActiveGrowthPoint[j]);
				}
				PointsToDelete.Add(ActiveGrowthPoint[j]);
			}
			else {
				if (ActiveGrowthPoint[j].Age > 4 && RandStream.FRand() < RandomWallSpawnChance && CanGenerateRandomWall(ActiveGrowthPoint[j])) {
					RandomWallPoints.Add(ActiveGrowthPoint[j]);
				}
				else if ((ActiveGrowthPoint[j].Age > MinAgeToAllowBranchSpawn && RandStream.FRand() < GetBranchProbability(ActiveGrowthPoint[j].Age) && CanBranch(ActiveGrowthPoint[j])) || (ActiveGrowthPoint[j].Age > AgeToForceBranchSpawn && CanBranch(ActiveGrowthPoint[j]))) {	//Create branch
					ActiveGrowthPoint[j].Age = 0;
					ActiveGrowthPoint[j].BranchesCreated++;
					FVector Dir = ActiveGrowthPoint[j].BranchDirection;
					ActiveGrowthPoint[j].BranchDirection = ActiveGrowthPoint[j].BranchDirection * -1;

					FVector BranchDir = RandStream.RandRange(0, 1) == 0 ? ActiveGrowthPoint[j].Direction : -1 * ActiveGrowthPoint[j].Direction;
					//Testing CollisionAvoidancMultiplierLength and CollisionAvoidancMultiplierWidth
					ActiveGrowthPoint.Add(FWallGrowthPoint(ActiveGrowthPoint[j].Position, Dir, 0, BranchDir, 0, RandStream.RandRange(CollisionAvoidanceValueMin, CollisionAvoidanceValueMax), RandStream.RandRange(CollisionAvoidanceValueMin, CollisionAvoidanceValueMax)));

					if (ActiveGrowthPoint[j].BranchesCreated > 3) {
						if (RandStream.FRand() < 0.3) {
							PointsToDelete.Add(ActiveGrowthPoint[j]);
						}
					}
				}
			}

			if (!PointsToDelete.Contains(ActiveGrowthPoint[j])) {
				if (!RandomWallPoints.Contains(ActiveGrowthPoint[j])) {
					PlaceWall(ActiveGrowthPoint[j].Position, ActiveGrowthPoint[j].Direction);
					OccupiedPositions.Add(ActiveGrowthPoint[j].Position);
					OccupiedPositions.Add(ActiveGrowthPoint[j].Position + ActiveGrowthPoint[j].Direction);

					ActiveGrowthPoint[j].Position = ActiveGrowthPoint[j].Position + ActiveGrowthPoint[j].Direction;
					ActiveGrowthPoint[j].Age++;
				}
			}
		}
		ActiveGrowthPoint.RemoveAll([&](const FWallGrowthPoint& Point) {
			return PointsToDelete.Contains(Point);
		});

		for (const FWallGrowthPoint& Point : RandomWallPoints) {
			if (ActiveGrowthPoint.Contains(Point)) {
				for (int32 i = RandomWallLength; i < RandomWallLength * 2; ++i) {
					PlaceWall(Point.Position + Point.Direction * i, Point.Direction);
					OccupiedPositions.Add(Point.Position + Point.Direction * i);
					OccupiedPositions.Add(Point.Position + Point.Direction * i + Point.Direction);
				}
				FWallGrowthPoint newPoint = Point;
				newPoint.Position = newPoint.Position + newPoint.Direction * RandomWallLength * 3;
				newPoint.Age = 0;
				ActiveGrowthPoint[ActiveGrowthPoint.Find(Point)] = newPoint;
			}
		}
		RandomWallPoints.Empty();
		//GetWorld()->GetTimerManager().SetTimer(TimeHandle, this, &AWallsGenerator::GenerateWalls, FMath::Max(MaxDelay, delay), false);
		GenerateWalls();
	}
}

void AWallsGenerator::SpawnWalls() {
	UE_LOG(LogTemp, Warning, TEXT("SpawnWalls"));
	for (FWallKey Key : PlacedWalls) {
		AActor* Wall = GetWorld()->SpawnActor<AActor>(WallClass, Key.Location, FRotator(0, Key.Rotation, 0));
	}
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AWallsGenerator::SpawnStart);
}

void AWallsGenerator::GenerateBorder() {
	TArray<FWallGrowthPoint> WorldCorners;
	TSet<FVector> PlacedSegmentsPositions;
	bool ShouldContinueGenerating = true;
	for (int32 i = 0; i < 4; ++i) {
		FVector Location;
		FVector Direction;
		if (i == 0) {
			Location = FVector(EdgeCoordingate, EdgeCoordingate, 0);
			Direction = Directions[3];
		}
		else if (i == 1) {
			Location = FVector(EdgeCoordingate, -EdgeCoordingate, 0);
			Direction = Directions[1];
		}
		else if (i == 2) {
			Location = FVector(-EdgeCoordingate, EdgeCoordingate, 0);
			Direction = Directions[0];
		}
		else {
			Location = FVector(-EdgeCoordingate, -EdgeCoordingate, 0);
			Direction = Directions[2];
		}
		WorldCorners.Add(FWallGrowthPoint(Location, Direction, 0, Direction, 0, RandStream.RandRange(CollisionAvoidanceValueMin, CollisionAvoidanceValueMax), RandStream.RandRange(CollisionAvoidanceValueMin, CollisionAvoidanceValueMax)));
	}
	while (ShouldContinueGenerating) {
		for (int32 i = 0; i < 4; ++i) {
			FWallGrowthPoint Point = WorldCorners[i];
			PlaceWall(Point.Position, Point.Direction);
			OccupiedPositions.Add(Point.Position);
			OccupiedPositions.Add(Point.Position + Point.Direction);
			PlacedSegmentsPositions.Add(Point.Position);
			WorldCorners[i] = FWallGrowthPoint(Point.Position + Point.Direction, Point.Direction, 0, Point.Direction, 0, RandStream.RandRange(CollisionAvoidanceValueMin, CollisionAvoidanceValueMax), RandStream.RandRange(CollisionAvoidanceValueMin, CollisionAvoidanceValueMax));
		}
		for (int32 i = 0; i < 4; ++i) {
			if (PlacedSegmentsPositions.Contains(WorldCorners[i].Position)) {
				ShouldContinueGenerating = false;
				break;
			}
		}
	}
	SpawnWalls();
}

void AWallsGenerator::SpawnStart() {
	if (LevelStartClass) {
		UE_LOG(LogTemp, Warning, TEXT("SpawnStart"));
		GetWorld()->SpawnActor<AActor>(LevelStartClass, FVector(RandStream.FRandRange(-EdgeCoordingate, EdgeCoordingate), -EdgeCoordingate + WallLength * RandStream.RandRange(4, 40), 0), FRotator(0, 0, 0));
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AWallsGenerator::SpawnExit);
	}
}

void AWallsGenerator::SpawnExit() {
	if (LevelExitClass) {
		GetWorld()->SpawnActor<AActor>(LevelExitClass, FVector(RandStream.FRandRange(-EdgeCoordingate, EdgeCoordingate), EdgeCoordingate - WallLength * RandStream.RandRange(4, 40), 0), FRotator(0, 0, 0));
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AWallsGenerator::SpawnColumns);
	}
}

void AWallsGenerator::SpawnColumns() {
	if (ColumnsGeneratorClass) {
		GetWorld()->SpawnActor<AActor>(ColumnsGeneratorClass, FVector(EdgeCoordingate, EdgeCoordingate, 0), FRotator(0, 0, 0));
	}
}

// Called every frame
void AWallsGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AWallsGenerator::CanBranch(const FWallGrowthPoint& GrowthPoint) const {
	if (EdgeCoordingate - FMath::Abs(GrowthPoint.Position.X) > WallLength * 3 && 
		EdgeCoordingate - FMath::Abs(GrowthPoint.Position.Y) > WallLength * 3) {
		return true;
	}
	return false;
}

bool AWallsGenerator::CanGenerateRandomWall(const FWallGrowthPoint& GrowthPoint) const {
	FWallGrowthPoint NewPoint = GrowthPoint;
	for (int32 i = 0; i <= RandomWallLength * 3; ++i) {
		NewPoint.Position = GrowthPoint.Position + GrowthPoint.Direction * i;
		if (IsAtTheEdge(NewPoint)) {
			return false;
		}
	}
	return true;
}

bool AWallsGenerator::IsAtTheEdge(const FWallGrowthPoint& GrowthPoint) const {
	if (FMath::Abs(GrowthPoint.Position.X) == EdgeCoordingate || FMath::Abs(GrowthPoint.Position.Y) == EdgeCoordingate) {
		return true;
	}
	return false;
}

bool AWallsGenerator::IsNearExistingWall(const FWallGrowthPoint& GrowthPoint) const{
	TSet<FVector> CollisionAvoidanceVectors;

	for (int32 i = 1; i < GrowthPoint.CollisionAvoidanceValueLength; ++i) {
		CollisionAvoidanceVectors.Add(GrowthPoint.Position + GrowthPoint.Direction * i);
	}

	TPair<FVector, FVector> PerpendicularVectors = GetPerpendicularDirections(GrowthPoint.Direction);
	for (int32 j = 1; j < GrowthPoint.CollisionAvoidanceValueWidth; ++j) {
		for (int32 i = 1; i < GrowthPoint.CollisionAvoidanceValueLength; ++i) {
			CollisionAvoidanceVectors.Add(GrowthPoint.Position + GrowthPoint.Direction * i + PerpendicularVectors.Key * j);
			CollisionAvoidanceVectors.Add(GrowthPoint.Position + GrowthPoint.Direction * i + PerpendicularVectors.Value * j);
		}
	}

	for (const FVector& CollisionAvoidanceVector : CollisionAvoidanceVectors) {
		if (OccupiedPositions.Contains(CollisionAvoidanceVector)) {
			return true;
		}
	}
	return false;
}

double AWallsGenerator::GetBranchProbability(int32 age) const{
	float MaxProbability = 0.5f;
	float Steepness = 1.0f;
	float BaseProbability = 0.03f;
	return FMath::Min((pow(2, age * Steepness) - 1) * BaseProbability, MaxProbability);
}

void AWallsGenerator::PlaceWall(FVector Location, FVector Direction) {
	float Rotation = GetWallRotation(Direction);
	if (WallClass) {
		FWallKey Key{Location, Rotation};
		if (!PlacedWalls.Contains(Key)) {
			
			//AActor* Wall = GetWorld()->SpawnActor<AActor>(WallClass, Location, Rotation);
			//PlacedWalls.Add(Key, Wall);
			PlacedWalls.Add(Key);
		}
	}
}

void AWallsGenerator::RemoveRecentWalls(const FWallGrowthPoint& GrowthPoint) {
	FVector DirBack = -GrowthPoint.Direction;
	FVector Current = GrowthPoint.Position;
	float Rotation = GetWallRotation(GrowthPoint.Direction);

	for (int32 i = 0; i <= GrowthPoint.Age; ++i) {
		FWallKey Key{Current, Rotation};

		if (PlacedWalls.Contains(Key)) {
			/*if (AActor* Wall = PlacedWalls[Key]) {
				Wall->Destroy();
				//FVector Location = Wall->GetActorLocation();
				//Wall->SetActorLocation(FVector(Location.X, Location.Y, Location.Z + 200));
			}*/
			PlacedWalls.Remove(Key);
			OccupiedPositions.Remove(Current);
		}
		Current += DirBack;
	}
}

TPair<FVector, FVector> AWallsGenerator::GetPerpendicularDirections(FVector dir) const {
	TPair<FVector, FVector> PerpendicularDirections(Directions[2], Directions[3]); // N and S directions
	if (dir == Directions[2] || dir == Directions[3]) {
		PerpendicularDirections = TPair<FVector, FVector>(Directions[0], Directions[1]);
	}
	return PerpendicularDirections;
}

double AWallsGenerator::GetWallRotation(FVector dir) const {
	if (dir == Directions[0])
		return 90.f;
	if (dir == Directions[1])
		return -90.f;
	if (dir == Directions[2])
		return 180.f;

	return 0.f;
}

float AWallsGenerator::SnapToGrid(float Value, float GridSize) {
	return FMath::RoundToInt(Value / GridSize) * GridSize;
}

float AWallsGenerator::GetColumnSpawnChance() {
	return ColumnSpawnChance;
}

int32 AWallsGenerator::GetWorldSeed() {
	return WorldSeed;
}