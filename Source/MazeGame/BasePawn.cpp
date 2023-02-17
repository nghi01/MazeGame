// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
ABasePawn::ABasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CapComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapComp;
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	BaseMesh->SetupAttachment(CapComp);
}

// Called when the game starts or when spawned
void ABasePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABasePawn::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ABasePawn::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ABasePawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &ABasePawn::AddControllerYawInput);
}
void ABasePawn::MoveForward(float Value){
	AddMovementInput(GetActorForwardVector(), Value);
}

void ABasePawn::MoveRight(float Value){
	AddMovementInput(GetActorRightVector(), Value);
}
