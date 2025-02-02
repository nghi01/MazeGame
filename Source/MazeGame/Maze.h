// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TriggerVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Maze.generated.h"

UCLASS()
class MAZEGAME_API AMaze : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMaze();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void ChangeScene();

private:
	AActor* Character;
	AActor* TriggerBox;
	FName WinPage = FName(TEXT("WinPage"));
	
	TArray<TArray<TArray<bool>>> Passages;
	TArray<TArray<int32>> Done;
	TArray<TArray<int32>> Frontier;
	TArray<TArray<int32>> Unexplored;
	TArray<int32> LastExploredLocation;
	
	int32 MapLength;

	const int32 EAST = 1;
	const int32 NORTH = 0;
	const int32 SOUTH = 2;
	const int32 WEST = 3;

	const int32 X = 0;
	const int32 Y = 1;

	const int32 OFFSETS[4][2] = { { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 } };

	void AddPassage(TArray<int32> A, TArray<int32> B);
	TArray<int32> ChooseRandomlyFrom(TArray<TArray<int32>> List);
	TArray<int32> ExpandLocation(TArray<int32> Here, int32 Direction);
	TArray<int32> ExpandMaze();
	void GenerateMaze();
	void SpawnFloor();
	void SpawnCorners();
	void SpawnWalls();
	void SpawnTriggerBox();

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> FloorClass;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> WallClass;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> CornerClass;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> TriggerBoxClass;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 MazeSize = 21;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 WallWidth = 100;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	double PassageWidthToWallWidthRatio = 2.0;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 Height = 400;
};
