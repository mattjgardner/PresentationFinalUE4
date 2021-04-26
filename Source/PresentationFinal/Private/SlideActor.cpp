// Fill out your copyright notice in the Description page of Project Settings.


#include "SlideActor.h"
#include "Misc/Paths.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "HAL/FileManager.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "OVRLipSyncPlaybackActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
ASlideActor::ASlideActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Sets the actor to replicate
	bReplicates = true;
	bAlwaysRelevant = true;

	//creates a cube
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	UStaticMesh* CubeMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'")).Object;
	Mesh->SetStaticMesh(CubeMesh);
	RootComponent = Mesh;
	//Scales the cube to make it more screen like
	Mesh->SetWorldScale3D(FVector(0.01, 4, 4));


	SlideIndex = 0;

//	//Directory to all materials and creates an array of paths to each material -> useful for slide changing
//	//only compiles this for the editor to stop crashes
//	//Second check was the original to check it wasnt a dedicated server but this isn't strictly necessary
//	//with the new #if WITH_EDITOR
	//Commented out due to packaging errors with the package build. However, the slide sounds still work with this code


//#if WITH_EDITOR
//	//Gets directory to game	
//	GameDirectory = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
//
//	//Finds all the materials for the slides in the specified folder
//	MaterialsDirectory = GameDirectory + "Slides/Materials/";
//	IFileManager::Get().FindFiles(MaterialPaths, MaterialsDirectory.GetCharArray().GetData());
//
//	for (FString withuasset : MaterialPaths)
//	{
//		withuasset.RemoveFromEnd(TEXT(".uasset"), ESearchCase::IgnoreCase);
//		FString MaterialPath = "/Game/Slides/Materials/" + withuasset + "." + withuasset;
//			
//	}
//
//
	//Finds all sounds in the specified folder
	//Fills array of slidesounds and slidetimes
	SoundsDirectory = GameDirectory + "Slides/Sounds/";
	IFileManager::Get().FindFiles(SoundPaths, SoundsDirectory.GetCharArray().GetData());

	for (FString withuasset : SoundPaths)
	{
		withuasset.RemoveFromEnd(TEXT(".uasset"), ESearchCase::IgnoreCase);
		FString SoundPath = "/Game/Slides/Sounds/" + withuasset + "." + withuasset;
		SlideSound = LoadObject<USoundBase>(nullptr, *SoundPath);

		if (SlideSound)
		{
			SlideSounds.Add(SlideSound);
			SlideTimes.Add(SlideSound->GetDuration());
		}

	}
//
//
//
//
//	//finds the first slide to apply to the cube before play and applies the material to the cube
//	//GetCharArray().GetData() turns FString into const TCHAR* array
//	FString withuasset;
//	if (MaterialPaths.Num() != 0)
//	{
//		withuasset = MaterialPaths[0];
//	}
//
//	withuasset.RemoveFromEnd(TEXT(".uasset"), ESearchCase::IgnoreCase);
//	slide1Directory = "/Game/Slides/Materials/" + withuasset + "." + withuasset;
//	static ConstructorHelpers::FObjectFinder<UMaterial> material(slide1Directory.GetCharArray().GetData());
//	Mesh->SetMaterial(0, (material.Object));
//
//#endif

	//Collision box for popping UI's when character overlaps

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("ColiisionBox");
	CollisionBox->SetupAttachment(RootComponent, NAME_None);
	CollisionBox->SetRelativeLocation(FVector(0, 0, 0), false, nullptr, ETeleportType::None);
	CollisionBox->SetRelativeScale3D(FVector(500, 2, 2));
	CollisionBox->SetGenerateOverlapEvents(true);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

// Called when the game starts or when spawned
void ASlideActor::BeginPlay()
{
	Super::BeginPlay();

	//Sets the material to the first slide on begin play after checking a material in the array exists
	//This means it is applied even outside of the editor when the game is opened
	if (Materials.Num() != 0)
	{
		Mesh->SetMaterial(0, Materials[0]);
	}


}

// Called every frame
void ASlideActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Starts the timer for 4 seconds before the slideshow starts
void ASlideActor::StartSlideshow()
{
	//TimerDelegate used to bind a delegate function to it, takes an array index and an array of materials in the function
	bIsStarted = true;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("SetSlide"), 0, Materials);
	
	GetWorldTimerManager().SetTimer(SlideTimerHandle, TimerDelegate, 4, false);
}

//Sets the material to the new slide
//Plays sound at the location of the mesh (the slide actor)
//if it's not the penultimate slide, sets a new timer to continue on the slideshow after 
//the whole sound has played plus a buffer time of 4 seconds
//Other wise it pauses the timer so game doesn't crash
void ASlideActor::SetSlide(int32 inSlideIndex, TArray<UMaterial*> inMaterials)
{
	UE_LOG(LogTemp, Log, TEXT("SetSlide"));
	if (inSlideIndex < inMaterials.Num())
	{
		UE_LOG(LogTemp, Log, TEXT("%i, %i"), inSlideIndex, inMaterials.Num());
		//FString withuasset = inMaterialPaths[inSlideIndex];
		//withuasset.RemoveFromEnd(TEXT(".uasset"), ESearchCase::IgnoreCase);
		//FString MaterialPath = "/Game/Slides/Materials/" + withuasset + "." + withuasset;
		//UMaterial* loadedmaterial = LoadObject<UMaterial>(nullptr, *MaterialPath);
		Mesh->SetMaterial(0, inMaterials[inSlideIndex]);
		FVector MeshLocation = Mesh->GetComponentLocation();
		if (bIsUsingOVRLipsync == false)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SlideSounds[inSlideIndex], MeshLocation);
		}
	}


	if (inSlideIndex + 1 < inMaterials.Num())
	{
		FTimerDelegate TimerDelegate;

		SlideIndex = inSlideIndex + 1;

		TimerDelegate.BindUFunction(this, FName("SetSlide"), SlideIndex, inMaterials);
		GetWorldTimerManager().SetTimer(SlideTimerHandle, TimerDelegate, SlideSounds[inSlideIndex--]->GetDuration() + 4, false);

	}
	else
	{
		GetWorldTimerManager().PauseTimer(SlideTimerHandle);
	}
}

//If the slideshow is started, it pauses or unpauses the slideshow
//by pausing the timer/unpausing the timer
//However the sound will continue playing
//When unpaused, timer will start again and play out before the next slide is played
void ASlideActor::PauseUnpauseSlideshow()
{
	if (bIsStarted == true)
	{
		if (bPaused == false)
		{
			GetWorldTimerManager().PauseTimer(SlideTimerHandle);
			bPaused = true;
		}
		else
		{
			if (GetWorldTimerManager().IsTimerPaused(SlideTimerHandle) == true)
			{
				GetWorldTimerManager().UnPauseTimer(SlideTimerHandle);
				bPaused = false;
			}
		}
	}
}

//Getters and setters for certain object properties
//Blueprintcallable so they can be called within blueprint graphs
int32 ASlideActor::GetSlideIndex()
{
	return SlideIndex;
}

void ASlideActor::SetSlideIndex(int32 inSlideIndex)
{
	SlideIndex = inSlideIndex;
}

bool ASlideActor::GetBoolIsStarted()
{
	return bIsStarted;
}

bool ASlideActor::GetBoolIsPaused()
{
	return bPaused;
}

void ASlideActor::GetSoundData(TArray<float> &inSlideTimes, TArray<USoundBase*> &inSlideSounds)
{
	inSlideTimes = SlideTimes;
	inSlideSounds = SlideSounds;
}

void ASlideActor::SetBoolIsUsingOVRLipSync()
{
	if (bIsUsingOVRLipsync == false)
	{
		bIsUsingOVRLipsync = true;
	}
	else
	{
		bIsUsingOVRLipsync = false;
	}
}

