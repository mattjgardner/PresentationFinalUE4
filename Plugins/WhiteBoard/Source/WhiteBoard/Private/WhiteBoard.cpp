// Copyright Epic Games, Inc. All Rights Reserved.

#include "WhiteBoard.h"
#include "UObject/UObjectGlobals.h"
#include "Engine/CanvasRenderTarget2D.h"
//#include "Factories/MaterialFactoryNew.h"
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

	FString WhiteboardMaterialPath = "Material'/WhiteBoard/Whiteboard_Mat.Whiteboard_Mat'";
	UMaterial* WhiteboardMaterial = LoadObject<UMaterial>(NULL, *WhiteboardMaterialPath);

	FString RenderTargetPath = "CanvasRenderTarget2D'/WhiteBoard/RenderTarget.RenderTarget'";
	UCanvasRenderTarget2D* RenderTarget = LoadObject<UCanvasRenderTarget2D>(NULL, *RenderTargetPath);

	FString WhiteboardPenPath = "Material'/WhiteBoard/WhiteboardPen_Mat.WhiteboardPen_Mat'";
	UMaterial* WhiteboardPen = LoadObject<UMaterial>(NULL, *WhiteboardPenPath);

	if (WhiteboardMaterial->Expressions.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO EXPRESSIONS Whiteboard!"));

		UMaterialExpressionTextureSampleParameter2D* TextureExpression = NewObject<UMaterialExpressionTextureSampleParameter2D>(WhiteboardMaterial);
		TextureExpression->Texture = RenderTarget;
		TextureExpression->SamplerType = SAMPLERTYPE_LinearColor;
		TextureExpression->ParameterName = "TextureExpression";
		
		UMaterialExpressionVectorParameter* BoardColour = NewObject<UMaterialExpressionVectorParameter>(WhiteboardMaterial);
		BoardColour->DefaultValue = FLinearColor::White;
		BoardColour->ParameterName = "BoardColour";

		UMaterialExpressionVectorParameter* DrawColour = NewObject<UMaterialExpressionVectorParameter>(WhiteboardMaterial);
		DrawColour->DefaultValue = FLinearColor::Black;
		DrawColour->ParameterName = "DrawColour";


		UMaterialExpressionMultiply* Multiply = NewObject<UMaterialExpressionMultiply>(WhiteboardMaterial);

		UMaterialExpressionScalarParameter* Opacity = NewObject<UMaterialExpressionScalarParameter>(WhiteboardMaterial);
		Opacity->DefaultValue = 0.5f;
		Opacity->ParameterName = "Opacity";

		UMaterialExpressionLinearInterpolate* Lerp = NewObject<UMaterialExpressionLinearInterpolate>(WhiteboardMaterial);

		WhiteboardMaterial->Expressions.Add(TextureExpression);
		WhiteboardMaterial->Expressions.Add(BoardColour);
		WhiteboardMaterial->Expressions.Add(DrawColour);
		WhiteboardMaterial->Expressions.Add(Multiply);
		WhiteboardMaterial->Expressions.Add(Opacity);

		Multiply->A.Connect(0, DrawColour);
		Multiply->B.Connect(0, TextureExpression);

		Lerp->A.Connect(0, BoardColour);
		Lerp->B.Connect(0, Multiply);
		Lerp->Alpha.Connect(0, TextureExpression);


		WhiteboardMaterial->Opacity.Expression = Opacity;
		WhiteboardMaterial->BaseColor.Expression = Lerp;
		WhiteboardMaterial->BlendMode = EBlendMode::BLEND_Translucent;

		WhiteboardMaterial->PreEditChange(NULL);
		WhiteboardMaterial->PostEditChange();
	}
	
	



	if (WhiteboardPen->Expressions.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO EXPRESSIONS Pen!"));
		UMaterialExpressionVectorParameter* XYVector = NewObject<UMaterialExpressionVectorParameter>(WhiteboardPen);
		XYVector->DefaultValue = 0.5 * FLinearColor::Yellow;
		

		UMaterialExpressionComponentMask* Mask = NewObject<UMaterialExpressionComponentMask>(WhiteboardPen);
		Mask->R = 1;
		Mask->G = 1;
		Mask->B = 0;
		Mask->A = 0;



		UMaterialExpressionTextureCoordinate* TexCoord = NewObject<UMaterialExpressionTextureCoordinate>(WhiteboardPen);
		UMaterialExpressionSubtract* Subtract = NewObject<UMaterialExpressionSubtract>(WhiteboardPen);
		UMaterialExpressionSubtract* Subtract2 = NewObject<UMaterialExpressionSubtract>(WhiteboardPen);
		UMaterialExpressionSubtract* Subtract3 = NewObject<UMaterialExpressionSubtract>(WhiteboardPen);


		UMaterialExpressionDistance* VectorLength = NewObject<UMaterialExpressionDistance>(WhiteboardPen);


		UMaterialExpressionConstant* ZeroCoordinate = NewObject<UMaterialExpressionConstant>(WhiteboardPen);
		ZeroCoordinate->R = 0;

		UMaterialExpressionConstant* One = NewObject<UMaterialExpressionConstant>(WhiteboardPen);
		One->R = 1;



		UMaterialExpressionOneMinus* OneMinus = NewObject<UMaterialExpressionOneMinus>(WhiteboardPen);

		UMaterialExpressionClamp* Clamp = NewObject<UMaterialExpressionClamp>(WhiteboardPen);

		UMaterialExpressionDivide* Divide = NewObject<UMaterialExpressionDivide>(WhiteboardPen);

		UMaterialExpressionScalarParameter* DrawSize = NewObject<UMaterialExpressionScalarParameter>(WhiteboardPen);
		DrawSize->SetParameterName("DrawSize");
		DrawSize->DefaultValue = 0.005;
		



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
		Subtract->A.Connect(0, TexCoord);
		Subtract->B.Connect(0, Mask);
		Subtract2->A.Connect(0, One);
		Subtract2->B.Connect(0, VectorLength);
		Subtract3->A.Connect(0, Subtract2);
		Subtract3->B.Connect(0, OneMinus);
		VectorLength->A.Connect(0, Subtract);
		VectorLength->B.Connect(0, ZeroCoordinate);
		Clamp->Input.Connect(0, Subtract3);
		Divide->A.Connect(0, Clamp);
		Divide->B.Connect(0, DrawSize);
		OneMinus->Input.Connect(0, DrawSize);

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