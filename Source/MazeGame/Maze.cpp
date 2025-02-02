// Fill out your copyright notice in the Description page of Project Settings.
#include "Maze.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AMaze::AMaze()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMaze::BeginPlay()
{
	Super::BeginPlay();
	Character = GetWorld()->GetFirstPlayerController()->GetPawn();
	GenerateMaze();
}

// Called every frame
void AMaze::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// if (!Character) {
	// 	Character = GetWorld()->GetFirstPlayerController()->GetPawn();
	// }
	if (TriggerBox && Character && TriggerBox->IsOverlappingActor(Character)) {
		ChangeScene();
	}
}

void AMaze::ChangeScene()
{
	UGameplayStatics::OpenLevel(this, WinPage);
}

//Add a passage from position A to position B
void AMaze::AddPassage(TArray<int32> A, TArray<int32> B)
{
	if ((B[0] - A[0] == 1) && (B[1] == A[1])) Passages[A[0]][A[1]][EAST] = true;
	else if ((B[0] - A[0] == -1) && (B[1] == A[1])) Passages[A[0]][A[1]][WEST] = true;
	else if ((B[1] - A[1] == 1) && (B[0] == A[0])) Passages[A[0]][A[1]][NORTH] = true;
	else if ((B[1] - A[1] == -1) && (B[0] == A[0])) Passages[A[0]][A[1]][SOUTH] = true;
}

//Choose an element randomly from a List
TArray<int32> AMaze::ChooseRandomlyFrom(TArray<TArray<int32>> List)
{
	if (List.Num() == 0) return {-10, -10};
	int Index = FMath::RandRange(0, List.Num() - 1);
	return List[Index];
}

//Expand the maze in a direction by 1 square
TArray<int32> AMaze::ExpandLocation(TArray<int32> Here, int32 Direction)
{
	//Set the Delta Expand = 0
	TArray<int32> There = {0, 0};

	//Expand the maze in that direction
	There[X] = Here[X] + OFFSETS[Direction][X];
	There[Y] = Here[Y] + OFFSETS[Direction][Y];

	//If not already passage, add a passage
	if (Unexplored.Find(There) != INDEX_NONE) 
	{
		AddPassage(Here, There);
		return There;
	} 
	else return {-10, -10};
}

TArray<int32> AMaze::ExpandMaze()
{
	// Set Here = Current Location (Should be the last explored location, else, equal to the starting entrance position)
	TArray<int32> Here;
	if (LastExploredLocation[0] == -10 && LastExploredLocation[1] == -10) Here = ChooseRandomlyFrom(Frontier);
	else Here = LastExploredLocation;
	
	//Choose a Random Direction
	int32 Direction = FMath::RandRange(0, 3);
	for (int32 i = 0; i < 4; i++) 
	{
		TArray<int32> There = ExpandLocation(Here, Direction); 

		//If expandable, then Expand to that direction 1 space
		if (!(There[0] == -10 && There[1] == -10))
		{
			Frontier.Insert(There, 0);
			Unexplored.Remove(There);
			return There;
		}

		//Else, continue to see another direction (in short, this will check for all 4 directions)
		Direction = (Direction + 1) % 4;
	}

	//If not expandable in all direction then return
	Done.Insert(Here, 0);
	Frontier.Remove(Here);
	return {-10, -10};
}

void AMaze::GenerateMaze()
{
	MapLength = WallWidth * (MazeSize * PassageWidthToWallWidthRatio + MazeSize + 1);

	TArray<TArray<bool>> Row;

	//Fill the 2D Array Row with arrays of booleans (This is indication whether the direction has wall or not. N = 0, E = 1, S = 2, W = 3, these are the index of
	//the boolean array) (basically, each array of boolean represent one square inside the maze, number of squares * one square = 1 row)
	Row.Init({false, false, false, false}, MazeSize);

	//Many rows = whole passage
	Passages.Init(Row, MazeSize);

	Frontier = {{ 0, 0 }};
	LastExploredLocation = {-10, -10};

	//Fill Unexplored with the whole maze, because none has been explored yet at the beginning
	for (int x = 0; x < MazeSize; x++) 
	{
		for (int y = 0; y < MazeSize; y++) 
		{
			Unexplored.Insert({ x, y }, 0);
		}
	}

	// 0,0 will be the entrance point, so remove it from unexplored
	Unexplored.Remove({0, 0});

	SpawnFloor();

	//Spawn a corner in each and every (supposedly) square of the maze
	SpawnCorners();

	// While not finished expanding the maze, keep expanding (Because we will fill the whole maze until nowhere is unexplored)
	while (Unexplored.Num() > 0) 
	{
		LastExploredLocation = ExpandMaze();
	}

	//Only spawn walls that are not obstructing the paths of the expanded maze
	SpawnWalls();

	//Spawn a trigger box at the exit
	SpawnTriggerBox();
}

void AMaze::SpawnTriggerBox()
{
	FVector SpawnScale;
	SpawnScale.X = WallWidth * PassageWidthToWallWidthRatio / 100.0;
	SpawnScale.Y = WallWidth * PassageWidthToWallWidthRatio / 100.0;
	SpawnScale.Z = 1.0;

	FVector SpawnLocation;
	SpawnLocation.X = MapLength - WallWidth * PassageWidthToWallWidthRatio / 2.0 - WallWidth;
	SpawnLocation.Y = MapLength - WallWidth * PassageWidthToWallWidthRatio / 2.0 - WallWidth;
	SpawnLocation.Z = 0.0;

	FRotator SpawnRotation;
	SpawnRotation.Roll = 0.0;
	SpawnRotation.Pitch = 0.0;
	SpawnRotation.Yaw = 0.0;

	TriggerBox = (AActor*) GetWorld()->SpawnActor<AActor>(TriggerBoxClass, SpawnLocation, SpawnRotation);
	TriggerBox->SetActorTransform(FTransform(SpawnRotation.Quaternion(), SpawnLocation, SpawnScale));
}


void AMaze::SpawnFloor()
{
	FVector SpawnScale;
	SpawnScale.X = MapLength / 200.0;
	SpawnScale.Y = MapLength / 200.0;
	SpawnScale.Z = 1.0;

	FVector SpawnLocation;
	SpawnLocation.X = MapLength / 2.0;
	SpawnLocation.Y = MapLength / 2.0;
	SpawnLocation.Z = -20.0;

	FRotator SpawnRotation;
	SpawnRotation.Roll = 0.0;
	SpawnRotation.Pitch = 0.0;
	SpawnRotation.Yaw = 0.0;

	GetWorld()->SpawnActor<AActor>(FloorClass, SpawnLocation, SpawnRotation)->SetActorTransform(FTransform(SpawnRotation.Quaternion(), SpawnLocation, SpawnScale));
}


//Spawning corners 21*21
void AMaze::SpawnCorners()
{
	FVector SpawnScale;
	SpawnScale.X = WallWidth / 200.0;
	SpawnScale.Y = WallWidth / 200.0;;
	SpawnScale.Z = Height / 800.0;

	FVector SpawnLocation;
	SpawnLocation.X = 0.0;
	SpawnLocation.Y = 0.0;
	SpawnLocation.Z = Height / 2;

	FRotator SpawnRotation;
	SpawnRotation.Roll = 0.0;
	SpawnRotation.Pitch = 0.0;
	SpawnRotation.Yaw = 0.0;

	for (int i = 0; i < MazeSize + 1; i++)
	{
		for (int j = 0; j < MazeSize + 1; j++)
		{
			SpawnLocation.X = i * WallWidth * (1 + PassageWidthToWallWidthRatio) + WallWidth / 2;
			SpawnLocation.Y = j * WallWidth * (1 + PassageWidthToWallWidthRatio) + WallWidth / 2;
			GetWorld()->SpawnActor<AActor>(CornerClass, SpawnLocation, SpawnRotation)->SetActorTransform(FTransform(SpawnRotation.Quaternion(), SpawnLocation, SpawnScale));
		}
	}
}

//Spawning Walls that are not obstructing the path of the maze
void AMaze::SpawnWalls()
{
	FVector SpawnScale;
	SpawnScale.X = WallWidth / 200.0;
	SpawnScale.Y = WallWidth * PassageWidthToWallWidthRatio / 400.0;;
	SpawnScale.Z = Height / 800.0;

	FVector SpawnLocation;
	SpawnLocation.X = 0.0;
	SpawnLocation.Y = 0.0;
	SpawnLocation.Z = Height / 2;

	FRotator SpawnRotation;
	SpawnRotation.Roll = 0.0;
	SpawnRotation.Pitch = 0.0;

	for (int i = 0; i < MazeSize; i++)
	{
		SpawnRotation.Yaw = 90.0;
		if (i != 0)
		{
			SpawnLocation.X = WallWidth * (i * (1 + PassageWidthToWallWidthRatio) + PassageWidthToWallWidthRatio / 2 + 1);
			SpawnLocation.Y = WallWidth / 2;
			GetWorld()->SpawnActor<AActor>(WallClass, SpawnLocation, SpawnRotation)->SetActorTransform(FTransform(SpawnRotation.Quaternion(), SpawnLocation, SpawnScale));
		}

		SpawnLocation.X = WallWidth * (i * (1 + PassageWidthToWallWidthRatio) + PassageWidthToWallWidthRatio / 2 + 1);
		SpawnLocation.Y = MapLength - WallWidth / 2;
		GetWorld()->SpawnActor<AActor>(WallClass, SpawnLocation, SpawnRotation)->SetActorTransform(FTransform(SpawnRotation.Quaternion(), SpawnLocation, SpawnScale));

		SpawnRotation.Yaw = 0.0;
		SpawnLocation.X = WallWidth / 2;
		SpawnLocation.Y = WallWidth * (i * (1 + PassageWidthToWallWidthRatio) + PassageWidthToWallWidthRatio / 2 + 1);
		GetWorld()->SpawnActor<AActor>(WallClass, SpawnLocation, SpawnRotation)->SetActorTransform(FTransform(SpawnRotation.Quaternion(), SpawnLocation, SpawnScale));

		if (i != MazeSize - 1)
		{
			SpawnLocation.X = MapLength - WallWidth / 2;
			SpawnLocation.Y = WallWidth * (i * (1 + PassageWidthToWallWidthRatio) + PassageWidthToWallWidthRatio / 2 + 1);
			GetWorld()->SpawnActor<AActor>(WallClass, SpawnLocation, SpawnRotation)->SetActorTransform(FTransform(SpawnRotation.Quaternion(), SpawnLocation, SpawnScale));
		}
	}

	for (int x = 0; x < Passages.Num(); x++)
	{
		for (int y = 0; y < Passages.Num(); y++)
		{
			if (!(Passages[x][y][NORTH] || ((y + 1 < Passages.Num()) && Passages[x][y + 1][SOUTH])))
			{
				SpawnRotation.Yaw = 90.0;
				SpawnLocation.X = WallWidth * (x * (1 + PassageWidthToWallWidthRatio) + PassageWidthToWallWidthRatio / 2 + 1);
				SpawnLocation.Y = (y + 1) * WallWidth * (1 + PassageWidthToWallWidthRatio) + WallWidth / 2;
				GetWorld()->SpawnActor<AActor>(WallClass, SpawnLocation, SpawnRotation)->SetActorTransform(FTransform(SpawnRotation.Quaternion(), SpawnLocation, SpawnScale));
			}
			if (!(Passages[x][y][EAST] || ((x + 1 < Passages.Num()) && Passages[x + 1][y][WEST])))
			{
				if (!((x == Passages.Num() - 1) && (y == Passages.Num() - 1)))
				{
					SpawnRotation.Yaw = 0.0;
					SpawnLocation.X = (x + 1) * WallWidth * (1 + PassageWidthToWallWidthRatio) + WallWidth / 2;
					SpawnLocation.Y = WallWidth * (y * (1 + PassageWidthToWallWidthRatio) + PassageWidthToWallWidthRatio / 2 + 1);
					GetWorld()->SpawnActor<AActor>(WallClass, SpawnLocation, SpawnRotation)->SetActorTransform(FTransform(SpawnRotation.Quaternion(), SpawnLocation, SpawnScale));
				}
			}
		}
	}
}
