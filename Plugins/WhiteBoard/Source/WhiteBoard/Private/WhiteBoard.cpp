// Copyright Epic Games, Inc. All Rights Reserved.

#include "WhiteBoard.h"
#include "UObject/UObjectGlobals.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionConstant2Vector.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionConstant4Vector.h"
#include "Materials/MaterialExpressionLinearInterpolate.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionComponentMask.h"
#include "Materials/MaterialExpressionSubtract.h"
#include "Materials/MaterialExpressionMaterialFunctionCall.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionParameter.h"
#include "Materials/MaterialExpressionOneMinus.h"
#include "Materials/MaterialExpressionClamp.h"
#include "Materials/MaterialExpressionDivide.h"
#include "Materials/MaterialLayersFunctions.h"
#include "Materials/MaterialExpressionDistance.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "FWhiteBoardModule"



void FWhiteBoardModule::StartupModule()
{
#if WITH_EDITOR
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	CreateWhiteboard();

#endif
}

void FWhiteBoardModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#if WITH_EDITOR
void FWhiteBoardModule::CreateWhiteboard()
{
	//Loads in the materials ready for editing

	FString WhiteboardMaterialPath = "Material'/WhiteBoard/Whiteboard_Mat.Whiteboard_Mat'";
	UMaterial* WhiteboardMaterial = LoadObject<UMaterial>(NULL, *WhiteboardMaterialPath);

	FString RenderTargetPath = "CanvasRenderTarget2D'/WhiteBoard/RenderTarget.RenderTarget'";
	UCanvasRenderTarget2D* RenderTarget = LoadObject<UCanvasRenderTarget2D>(NULL, *RenderTargetPath);

	FString WhiteboardPenPath = "Material'/WhiteBoard/WhiteboardPen_Mat.WhiteboardPen_Mat'";
	UMaterial* WhiteboardPen = LoadObject<UMaterial>(NULL, *WhiteboardPenPath);

	//if there's no expressions in the material graph, it will make some for the whiteboard to work

	if (WhiteboardMaterial->Expressions.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO EXPRESSIONS Whiteboard!"));


		//Texture expression node takes a texture to be connected to the material, in this case, the render target

		UMaterialExpressionTextureSampleParameter2D* TextureExpression = NewObject<UMaterialExpressionTextureSampleParameter2D>(WhiteboardMaterial);
		TextureExpression->Texture = RenderTarget;
		TextureExpression->SamplerType = SAMPLERTYPE_LinearColor;
		TextureExpression->ParameterName = "TextureExpression";
		

		//Create 2 vector parameters for colours of the board and drawing
		//These could change at runtime if a UI was created

		UMaterialExpressionVectorParameter* BoardColour = NewObject<UMaterialExpressionVectorParameter>(WhiteboardMaterial);
		BoardColour->DefaultValue = FLinearColor::White;
		BoardColour->ParameterName = "BoardColour";

		UMaterialExpressionVectorParameter* DrawColour = NewObject<UMaterialExpressionVectorParameter>(WhiteboardMaterial);
		DrawColour->DefaultValue = FLinearColor::Black;
		DrawColour->ParameterName = "DrawColour";

		//creates a multiply node

		UMaterialExpressionMultiply* Multiply = NewObject<UMaterialExpressionMultiply>(WhiteboardMaterial);

		//creates a scalar parameter for opacity allowing it to change at run time

		UMaterialExpressionScalarParameter* Opacity = NewObject<UMaterialExpressionScalarParameter>(WhiteboardMaterial);
		Opacity->DefaultValue = 0.5f;
		Opacity->ParameterName = "Opacity";

		//Creates a linear interpolate node

		UMaterialExpressionLinearInterpolate* Lerp = NewObject<UMaterialExpressionLinearInterpolate>(WhiteboardMaterial);

		//Adds all the expressions to the material

		WhiteboardMaterial->Expressions.Add(TextureExpression);
		WhiteboardMaterial->Expressions.Add(BoardColour);
		WhiteboardMaterial->Expressions.Add(DrawColour);
		WhiteboardMaterial->Expressions.Add(Multiply);
		WhiteboardMaterial->Expressions.Add(Opacity);

		//Connects the multiply node to the texture expression and draw colour,
		//any where that is white on the render target is multiplied by the draw colour so the draw colour is expressed

		Multiply->A.Connect(0, DrawColour);
		Multiply->B.Connect(0, TextureExpression);

		//Anywhere that is black is lerped with the board colour causing it to become white in this case

		Lerp->A.Connect(0, BoardColour);
		Lerp->B.Connect(0, Multiply);
		Lerp->Alpha.Connect(0, TextureExpression);



		//connects the base colour to the output of the lerp node and the opacity to the opacity parameter
		//changes the blend mode so the opacity can be set
		WhiteboardMaterial->Opacity.Expression = Opacity;
		WhiteboardMaterial->BaseColor.Expression = Lerp;
		WhiteboardMaterial->BlendMode = EBlendMode::BLEND_Translucent;


		//Make sure editor knows about the changes
		WhiteboardMaterial->PreEditChange(NULL);
		WhiteboardMaterial->PostEditChange();
	}
	
	



	if (WhiteboardPen->Expressions.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO EXPRESSIONS Pen!"));


		//Vector for specifying where to draw to
		UMaterialExpressionVectorParameter* XYVector = NewObject<UMaterialExpressionVectorParameter>(WhiteboardPen);
		XYVector->DefaultValue = 0.5 * FLinearColor::Yellow;
		

		//Masks node to only allow R and G channels through, simulating X and Y parameters
		UMaterialExpressionComponentMask* Mask = NewObject<UMaterialExpressionComponentMask>(WhiteboardPen);
		Mask->R = 1;
		Mask->G = 1;
		Mask->B = 0;
		Mask->A = 0;


		//TexCoord graph exression created. The tex coord graph allows for drawing at a specific point.
		//Top left is (0,0)

		UMaterialExpressionTextureCoordinate* TexCoord = NewObject<UMaterialExpressionTextureCoordinate>(WhiteboardPen);
		UMaterialExpressionSubtract* Subtract = NewObject<UMaterialExpressionSubtract>(WhiteboardPen);
		UMaterialExpressionSubtract* Subtract2 = NewObject<UMaterialExpressionSubtract>(WhiteboardPen);
		UMaterialExpressionSubtract* Subtract3 = NewObject<UMaterialExpressionSubtract>(WhiteboardPen);

		//Gets the length of the vector
		UMaterialExpressionDistance* VectorLength = NewObject<UMaterialExpressionDistance>(WhiteboardPen);

		//Constant used to find the vector length from 0
		UMaterialExpressionConstant* ZeroCoordinate = NewObject<UMaterialExpressionConstant>(WhiteboardPen);
		ZeroCoordinate->R = 0;

		//Constant used to subtract from one
		UMaterialExpressionConstant* One = NewObject<UMaterialExpressionConstant>(WhiteboardPen);
		One->R = 1;


		//1 minus node does 1 minus whatever is inputted
		UMaterialExpressionOneMinus* OneMinus = NewObject<UMaterialExpressionOneMinus>(WhiteboardPen);

		//Clamps values to range i.e. 0 and 1
		UMaterialExpressionClamp* Clamp = NewObject<UMaterialExpressionClamp>(WhiteboardPen);

		//Divides A by B
		UMaterialExpressionDivide* Divide = NewObject<UMaterialExpressionDivide>(WhiteboardPen);


		//Parameter to specify the draw size of the pen
		UMaterialExpressionScalarParameter* DrawSize = NewObject<UMaterialExpressionScalarParameter>(WhiteboardPen);
		DrawSize->SetParameterName("DrawSize");
		DrawSize->DefaultValue = 0.005;
		

		//Add all expressions to graph and connect them

		WhiteboardPen->Expressions.Add(XYVector);
		WhiteboardPen->Expressions.Add(Mask);
		WhiteboardPen->Expressions.Add(TexCoord);
		WhiteboardPen->Expressions.Add(Subtract);
		WhiteboardPen->Expressions.Add(Subtract2);
		WhiteboardPen->Expressions.Add(Subtract3);
		WhiteboardPen->Expressions.Add(VectorLength);
		WhiteboardPen->Expressions.Add(ZeroCoordinate);
		WhiteboardPen->Expressions.Add(One);
		WhiteboardPen->Expressions.Add(DrawSize);
		WhiteboardPen->Expressions.Add(OneMinus);
		WhiteboardPen->Expressions.Add(Clamp);
		WhiteboardPen->Expressions.Add(Divide);

		Mask->Input.Connect(0, XYVector);

		//Takes away the draw location from the texture coordinate map,
		//images in personal documentation may help to understand what is going on
		//Top left is 0,0. The more right you go the more red the colour, the more
		//down you go the more green the colour.
		//If the coordinates subtracted are (0.5,0.5) from top left clockwise
		//black square, red square, brown square, green square
		Subtract->A.Connect(0, TexCoord);
		Subtract->B.Connect(0, Mask);
		
		//finds the vector length of all points
		//shows a spot in the centre that is blurry
		VectorLength->A.Connect(0, Subtract);
		VectorLength->B.Connect(0, ZeroCoordinate);

		//1 - length of each coordinate 
		//shows values very close to 1 in all cases
		Subtract2->A.Connect(0, One);
		Subtract2->B.Connect(0, VectorLength);

		//1 - DrawSize is a number quite close to 1
		//in pretty much all cases (drawsize max
		//value is 0.2)
		OneMinus->Input.Connect(0, DrawSize);

		//Subtract 3 outputs numbers very close to 0
		//(All the values that aren't on the draw location
		//are negative)
		Subtract3->A.Connect(0, Subtract2);
		Subtract3->B.Connect(0, OneMinus);

		//Clamp removes these negative values
		Clamp->Input.Connect(0, Subtract3);

		//Dividing by the draw size scales these values
		//so all numbers that are 0 -> 0, any other value
		//tends to 1, so a white point is drawn on the render
		//target
		Divide->A.Connect(0, Clamp);
		Divide->B.Connect(0, DrawSize);
		

		//Unlit model to allow for emissive colour to be set
		//Blend_Additive to allow for multiple drawings to be drawn to it, else old ones will be deleted
		WhiteboardPen->EmissiveColor.Expression = Divide;
		WhiteboardPen->SetShadingModel(EMaterialShadingModel::MSM_Unlit);
		WhiteboardPen->BlendMode = TEnumAsByte < EBlendMode>(EBlendMode::BLEND_Additive);
		WhiteboardPen->UpdateLightmassTextureTracking();

		WhiteboardPen->PreEditChange(NULL);
		WhiteboardPen->PostEditChange();
	}
}



#endif

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWhiteBoardModule, WhiteBoard)