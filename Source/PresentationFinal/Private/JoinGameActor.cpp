// Fill out your copyright notice in the Description page of Project Settings.


#include "JoinGameActor.h"

// Sets default values

//I was going to create an actor for joining games that users could enter and it would pop a UI
//However this was a lot quicker in blueprints so I went with that option instead
//UE4 can be a bit tricky with deleting classes so if this is still in, I left it
AJoinGameActor::AJoinGameActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJoinGameActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJoinGameActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

