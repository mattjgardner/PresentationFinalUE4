// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardActor.h"
#include "UObject/UObjectGlobals.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "WhiteBoard.h"
#include "Blueprint/WidgetTree.h"
#include "MyUserWidget.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABoardActor::ABoardActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bAlwaysRelevant = true;
	bReplicates = true;

	bReadyToDraw = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;

	UStaticMesh* CubeMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'")).Object;
	Mesh->SetStaticMesh(CubeMesh);
	Mesh->SetWorldScale3D(FVector(0.01, 4, 4));
	Mesh->SetRelativeLocation(FVector(0, 0, 0), false, nullptr, ETeleportType::None);


	BoardMaterial = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/WhiteBoard/Whiteboard_Mat.Whiteboard_Mat'")).Object;

	Mesh->SetMaterial(0, BoardMaterial);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("ColiisionBox");
	CollisionBox->SetupAttachment(RootComponent, NAME_None);
	CollisionBox->SetRelativeLocation(FVector(0, 0, 0), false, nullptr, ETeleportType::None);
	CollisionBox->SetRelativeScale3D(FVector(3000, 2, 2));
	CollisionBox->SetGenerateOverlapEvents(true);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	

	//OnRep_SetUpBoard();
}

// Called when the game starts or when spawned
void ABoardActor::BeginPlay()
{
	Super::BeginPlay();


	OnRep_SetUpBoard();
}

void ABoardActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

// Called every frame
void ABoardActor::Tick(float DeltaTime)
{
	
	Super::Tick(DeltaTime);
}


//Paints to render target at the location specified
void ABoardActor::Paint(FVector2D PaintLocation)
{

	XYCoords = FLinearColor(PaintLocation.X, PaintLocation.Y, 0, 1);
	if (bReadyToDraw)
	{
		if (PenMI)
		{
			PenMI->SetVectorParameterValue("Param", XYCoords);
			UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, RenderTarget, PenMI);
		}
	}
}

//Inverts the colour of the board, draw colour becomes board colour and vice versa
void ABoardActor::OnRep_OnSetInvert()
{
	FLinearColor TempDrawColour;
	/*BoardMaterialMI->GetVectorParameterValue("DrawColour", TempDrawColour);
	BoardMaterialMI->GetVectorParameterValue("BoardColour");*/
	TempDrawColour = BoardMaterialMI->K2_GetVectorParameterValue("DrawColour");
	BoardMaterialMI->SetVectorParameterValue("DrawColour", BoardMaterialMI->K2_GetVectorParameterValue("BoardColour"));
	BoardMaterialMI->SetVectorParameterValue("BoardColour", TempDrawColour);
}



//Set's up Whiteboard so all user's have a version of it to allow for variable replication which allows for collaboration across networks
void ABoardActor::OnRep_SetUpBoard()
{
	//do something
	WhiteboardPen = LoadObject<UMaterial>(NULL, TEXT("Material'/WhiteBoard/WhiteboardPen_Mat.WhiteboardPen_Mat'"));
	PenMI = UMaterialInstanceDynamic::Create(WhiteboardPen, this);
	BoardMaterialMI = UMaterialInstanceDynamic::Create(BoardMaterial, this);
	Mesh->SetMaterial(0, BoardMaterialMI);
	RenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(), 1024, 1024); //1024,1024
	RenderTarget->RenderTargetFormat = RTF_RGBA16f;
	RenderTarget->ClearColor = FLinearColor::Black;
	RenderTarget->UpdateResourceImmediate(true);
	PenMI = UMaterialInstanceDynamic::Create(WhiteboardPen, this);
	BoardMaterialMI->SetTextureParameterValue("TextureExpression", RenderTarget);

	bReadyToDraw = true;
}

//Called in PresentationFinalCharacter (PFC) to invert the colour of the whiteboard
//Prints out to 
bool ABoardActor::Client_OnRep_SetInvertColour_Validate(bool InbInvertColour)
{
	return true;
}


void ABoardActor::Client_OnRep_SetInvertColour_Implementation(bool InbInvertColour)
{
	bInvertColour = InbInvertColour;
	if (bReadyToDraw)
	{
		if (bInvertColour == true)
		{
			if (BoardMaterialMI)
			{
				FLinearColor TempDrawColour;
				TempDrawColour = BoardMaterialMI->K2_GetVectorParameterValue("DrawColour");
				BoardMaterialMI->SetVectorParameterValue("DrawColour", BoardMaterialMI->K2_GetVectorParameterValue("BoardColour"));
				BoardMaterialMI->SetVectorParameterValue("BoardColour", TempDrawColour);
			}
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("BoardActor Invert: %s"), bInvertColour ? TEXT("true") : TEXT("false"));
}


//Sets the opacity of the board, checking the material instance exists first
void ABoardActor::SetBoardOpacity(float inOpacity)
{
	if (bReadyToDraw)
	{
		if (BoardMaterialMI)
		{
			BoardMaterialMI->SetScalarParameterValue("Opacity", inOpacity);
		}
	}
}

//sets the draw size of the pen
void ABoardActor::SetDrawSize(float inDrawSize)
{
	if (bReadyToDraw)
	{
		if (PenMI)
		{
			PenMI->SetScalarParameterValue("DrawSize", inDrawSize);
		}
	}
}

//Clears the render target to wipe the board
void ABoardActor::ClearBoard()
{
	if (bReadyToDraw)
	{
		UKismetRenderingLibrary::ClearRenderTarget2D(this, RenderTarget, FLinearColor::Black);
	}
}



