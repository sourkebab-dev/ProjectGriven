// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
// Copyright 2020 LunaticX, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Animation/MorphTarget.h"
#include "Async/TaskGraphInterfaces.h"
#include "RuntimeCopyMorphToolsBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
DECLARE_DYNAMIC_DELEGATE(FDelegate);



USTRUCT()
struct FSameVerticesSet
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		TSet<uint32> SameVertices;
};



USTRUCT()
struct FVertexWeight
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		bool HaveAnyWeight;

	UPROPERTY()
	int32 FirstVertexIndex;

	UPROPERTY()
	float FirstVertexWeight;

	UPROPERTY()
	int32 SecondVertexIndex;

	UPROPERTY()
	float SecondVertexWeight;

	UPROPERTY()
	int32 ThirdVertexIndex;

	UPROPERTY()
	float ThirdVertexWeight;

};



USTRUCT()
struct FVertexWeightMap
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
	TArray<FVertexWeight> VertexWeights;

	UPROPERTY()
	TArray<FSameVerticesSet> TargetMeshSameVerticesSetArr;

	UPROPERTY()
	TMap<uint32, uint32> TargetToBaseMeshSameVertexMap;

};



struct FCloneMorphLODModel
{
	int32 NumMeshVerts;

	TArray<int32> SectionIndices;
	
	TArray<FMorphTargetDelta> Vertices;
};



struct FCloneMorph
{
	FString MorphName;

	TArray<FCloneMorphLODModel> CloneMorphLODModels;

	float MorphWeight;
};



USTRUCT()
struct FSaveMorphDelta
{
	GENERATED_USTRUCT_BODY()


		UPROPERTY()
		FVector PositionDelta;

	UPROPERTY()
		uint32 SourceIdx;

	UPROPERTY()
		FVector TangentZDelta;

};



USTRUCT()
struct FSaveMorphLODModel
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		int32 NumMeshVerts;

	UPROPERTY()
		TArray<int32> SectionIndices;

	UPROPERTY()
		TArray<FSaveMorphDelta> Vertices;


};



USTRUCT()
struct FSaveMorph
{
	GENERATED_USTRUCT_BODY()


		UPROPERTY()
		FString MorphName;

	UPROPERTY()
		TArray<FSaveMorphLODModel> SaveMorphLODModels;


};



USTRUCT()
struct FSaveMorphs
{
	GENERATED_USTRUCT_BODY()


		UPROPERTY()
		TArray<FSaveMorph> Morphs;
};



struct FCloneLODRenderDataVertex
{
	FVector VertexPosition;

	FVector TangentZ;
};



struct FCloneLODRenderData
{	
	int32 RenderSectionsNum;

	TArray<FCloneLODRenderDataVertex> Vertices;
};



struct FCloneLODIndices
{
	TArray<uint32> LODIndices;
};



UCLASS()
class UInitMorphsAndResourcesUtility : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
		USkeletalMesh* TargetMesh;

	//UPROPERTY()
		//USkeletalMesh* CopyFromMesh;

	UPROPERTY()
		TMap<FString, UMorphTarget*> MorphNameAndMorphTargetMap;

	UPROPERTY()
		TArray<FString> MorphNamesToRemove;

	//UPROPERTY()
		//TArray<float> MorphTargetWeights;

	//UPROPERTY()
		//FString NewMorphName;

	UPROPERTY()
		FDelegate Delegate;

	//UPROPERTY()
		//TArray<FString> MorphNames;

	//UPROPERTY()
		//FString FileName;

	//UPROPERTY()
		//bool OverWrite;

	UFUNCTION()
		void InitMorphsAndResources();

};



UCLASS()
class UAsyncTransferMorphsUtility : public UObject
{
	GENERATED_BODY()

public:

	TArray<FCloneMorph> CloneMorphs;

	UPROPERTY()
		USkeletalMesh* TargetMesh;

	UPROPERTY()
		FDelegate Delegate;

	UPROPERTY()
		TArray<FString> MorphNamesToRemove;

	UPROPERTY()
		TSet<FName> MorphNames;

	UPROPERTY()
		bool Overwrite;

	UFUNCTION()
		void AsyncTransferMorphs();

};



UCLASS()
class UAsyncSaveMorphsUtility : public UObject
{
	GENERATED_BODY()

public:

		TArray<FCloneMorph> CloneMorphs;

	UPROPERTY()
		FDelegate Delegate;

	UPROPERTY()
		FString FileName;

	UFUNCTION()
		void AsyncSaveMorphs();

};



UCLASS()
class UAsyncLoadMorphsUtility : public UObject
{
	GENERATED_BODY()

public:

	TArray<FString> CloneMorphNames;

	UPROPERTY()
		USkeletalMesh* TargetMesh;

	UPROPERTY()
		FDelegate Delegate;

	UPROPERTY()
		FString FileName;

	UPROPERTY()
		bool Overwrite;

	UFUNCTION()
		void AsyncLoadMorphs();

};



UCLASS()
class UAsyncBakeMorphsUtility : public UObject
{
	GENERATED_BODY()

public:

	TArray<FCloneMorph> CloneMorphs;

		TArray<FCloneLODRenderData> CloneLODRenderDatas;

	UPROPERTY()
		USkeletalMesh* TargetMesh;

	UPROPERTY()
		FString NewMorphName;

	UPROPERTY()
		FDelegate Delegate;

	UFUNCTION()
		void AsyncBakeMorphs();

};



UCLASS()
class UAsyncCopyMorphsUtility : public UObject
{
	GENERATED_BODY()

public:

	TArray<FCloneLODRenderData> TargetCloneLODRenderDatas;

	TArray<FCloneLODIndices> TargetCloneLODsIndices;

	TArray<FCloneMorph> CopyFromCloneMorphs;

	TArray<FCloneLODRenderData> CopyFromCloneLODRenderDatas;

	TArray<FCloneLODIndices> CopyFromCloneLODsIndices;

	UPROPERTY()
		USkeletalMesh* TargetMesh;

	UPROPERTY()
		int32 SmoothCount;

	UPROPERTY()
		int32 AdditionalMultiplyCount;

	UPROPERTY()
		float UniformInflateMagnitude;

	UPROPERTY()
		float Threshold;

	UPROPERTY()
		float SameVerticesThreshold;

	UPROPERTY()
		TArray<FString> MorphNamesToRemove;

	UPROPERTY()
		TMap<FString, UMorphTarget*> NewMorphTargetsMap;

	//UPROPERTY()
		//TArray<FString> MorphNames;

	UPROPERTY()
		FString VertexWeightMapFileName;

	UPROPERTY()
		FDelegate Delegate;

	UPROPERTY()
		bool Overwrite;

	UFUNCTION()
		void AsyncCopyMorphs();

};


/*
UCLASS()
class UAsyncUniformInflateMorphsUtility : public UObject
{
	GENERATED_BODY()

public:

	TArray<FCloneMorph> CloneMorphs;

	TArray<FCloneLODRenderData> CloneLODRenderDatas;

	UPROPERTY()
		USkeletalMesh* TargetMesh;

	UPROPERTY()
		FDelegate Delegate;

	UFUNCTION()
		void AsyncUniformInflateMorphs();

};
*/


UCLASS()
class URuntimeCopyMorphToolsSingleton : public UObject
{
	GENERATED_BODY()

public:

		TArray<FGraphEventRef> AsyncCopyMorphsTasks;
};



/*
class FAsyncExecTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FAsyncExecTask>;

public:
	int32 MyInput;
	TArray<FCloneLODRenderData>& InputTargetCloneLODRenderDatas;
	TArray<FCloneLODIndices>& InputTargetCloneLODsIndices;
	TArray<FCloneMorph>& InputCopyFromCloneMorphs;
	TArray<FCloneLODRenderData>& InputCopyFromCloneLODRenderDatas;
	TArray<FCloneLODIndices>& InputCopyFromCloneLODsIndices;
	USkeletalMesh* InputTargetMesh;
	int32 InputSmoothCount;
	int32 InputAdditionalMultiplyCount;
	float InputThreshold;
	float InputSameVerticesThreshold;
	TArray<FString>& InputMorphNamesToRemove;
	TMap<FString, UMorphTarget*>& InputNewMorphTargetsMap;
	FDelegate InputDelegate;
	bool InputOverwrite;
	FAsyncExecTask(TFunction<void(TArray<FCloneLODRenderData> TargetCloneLODRenderDatas, TArray<FCloneLODIndices> TargetCloneLODsIndices,
		TArray<FCloneMorph> CopyFromCloneMorphs, TArray<FCloneLODRenderData> CopyFromCloneLODRenderDatas, TArray<FCloneLODIndices> CopyFromCloneLODsIndices,
		USkeletalMesh* TargetMesh, int32 SmoothCount, int32 AdditionalMultiplyCount, float Threshold, float SameVerticesThreshold,
		TArray<FString> MorphNamesToRemove, TMap<FString, UMorphTarget*> NewMorphTargetsMap, FDelegate Delegate, bool Overwrite)> InWork,
		TArray<FCloneLODRenderData> TargetCloneLODRenderDatas, TArray<FCloneLODIndices> TargetCloneLODsIndices,
		TArray<FCloneMorph> CopyFromCloneMorphs, TArray<FCloneLODRenderData> CopyFromCloneLODRenderDatas, TArray<FCloneLODIndices> CopyFromCloneLODsIndices,
		USkeletalMesh* TargetMesh, int32 SmoothCount, int32 AdditionalMultiplyCount, float Threshold, float SameVerticesThreshold,
		TArray<FString> MorphNamesToRemove, TMap<FString, UMorphTarget*> NewMorphTargetsMap, FDelegate Delegate, bool Overwrite) :
		Work(InWork),
		InputTargetCloneLODRenderDatas(TargetCloneLODRenderDatas),
		InputTargetCloneLODsIndices(TargetCloneLODsIndices),
		InputCopyFromCloneMorphs(CopyFromCloneMorphs),
		InputCopyFromCloneLODRenderDatas(CopyFromCloneLODRenderDatas),
		InputCopyFromCloneLODsIndices(CopyFromCloneLODsIndices),
		InputTargetMesh(TargetMesh),
		InputSmoothCount(SmoothCount),
		InputAdditionalMultiplyCount(AdditionalMultiplyCount),
		InputThreshold(Threshold),
		InputSameVerticesThreshold(SameVerticesThreshold),
		InputMorphNamesToRemove(MorphNamesToRemove),
		InputNewMorphTargetsMap(NewMorphTargetsMap),
		InputDelegate(Delegate),
		InputOverwrite(Overwrite)
	{
	}

	void DoWork()
	{
		Work(InputTargetCloneLODRenderDatas,
			InputTargetCloneLODsIndices,
			InputCopyFromCloneMorphs,
			InputCopyFromCloneLODRenderDatas,
			InputCopyFromCloneLODsIndices,
			InputTargetMesh,
			InputSmoothCount,
			InputAdditionalMultiplyCount,
			InputThreshold,
			InputSameVerticesThreshold,
			InputMorphNamesToRemove,
			InputNewMorphTargetsMap,
			InputDelegate,
			InputOverwrite);
	}


	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FAsyncExecTask, STATGROUP_ThreadPoolAsyncTasks);
	}

private:

	TFunction<void(TArray<FCloneLODRenderData> TargetCloneLODRenderDatas, TArray<FCloneLODIndices> TargetCloneLODsIndices,
		TArray<FCloneMorph> CopyFromCloneMorphs, TArray<FCloneLODRenderData> CopyFromCloneLODRenderDatas, TArray<FCloneLODIndices> CopyFromCloneLODsIndices,
		USkeletalMesh* TargetMesh, int32 SmoothCount, int32 AdditionalMultiplyCount, float Threshold, float SameVerticesThreshold,
		TArray<FString> MorphNamesToRemove, TMap<FString, UMorphTarget*> NewMorphTargetsMap, FDelegate Delegate, bool Overwrite)> Work;

};
*/


UCLASS()
class URuntimeCopyMorphToolsBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:

	//UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Sample function", Keywords = "RuntimeCopyMorphTools sample test testing"), Category = "RuntimeCopyMorphToolsTesting")
	//static float RuntimeCopyMorphToolsSampleFunction(float Param);

	static FString GetRenameString();

	static URuntimeCopyMorphToolsSingleton* RuntimeCopyMorphToolsSingleton;

	//UFUNCTION(BluePrintCallable, Category = "RuntimeCopyMorphTools")
	//static void debugfunc(USkeletalMesh* TargetMesh);

	UFUNCTION(BluePrintCallable, Category = "RuntimeCopyMorphTools")
		static void RemoveMorphs(USkeletalMesh* TargetMesh, TArray<FString> MorphNames);

	UFUNCTION(BluePrintCallable, Category = "RuntimeCopyMorphTools")
		static void RenameMorph(USkeletalMesh* TargetMesh, FString OldMorphName, FString NewMorphName);

	UFUNCTION(BluePrintCallable, Category = "RuntimeCopyMorphTools")
		static void TransferMorphs(USkeletalMesh* TargetMesh, USkeletalMesh* TranferFromMesh, TArray<FString> MorphNames, FDelegate Done, bool Overwrite);

	UFUNCTION(BluePrintCallable, Category = "RuntimeCopyMorphTools")
		static void SaveMorphs(USkeletalMesh* TargetMesh, TArray<FString> MorphNames, FString FileName, FDelegate Done);

	UFUNCTION(BluePrintCallable, Category = "RuntimeCopyMorphTools")
		static void LoadMorphs(USkeletalMesh* TargetMesh, FString FileName, FDelegate Done, bool Overwrite);

	UFUNCTION(BluePrintCallable, Category = "RuntimeCopyMorphTools")
		static void BakeMorphs(USkeletalMeshComponent* TargetMeshComponent, TArray<FString> MorphNames, FString NewMorphName, FDelegate Done);

	UFUNCTION(BluePrintCallable, Category = "RuntimeCopyMorphTools", meta = (AdvancedDisplay = "Threshold, SameVerticesThreshold, SmoothCount, AdditionalMultiplyCount, UniformInflateMagnitude"))
		static void CopyMorphs(USkeletalMesh* TargetMesh, USkeletalMesh* CopyFromMesh, TArray<FString> MorphNames, FString VertexWeightMapFileName, FDelegate Done, float Threshold = 20, float SameVerticesThreshold = 0.001, int32 SmoothCount = 2, int32 AdditionalMultiplyCount = 0, float UniformInflateMagnitude = 0, bool Overwrite = false);

	//UFUNCTION(BluePrintCallable, Category = "RuntimeCopyMorphTools")
		//static void UniformInflateMorphs(USkeletalMesh* TargetMesh, TArray<FString> MorphNames, float Magnitude, FDelegate Done);

	UFUNCTION()
		static URuntimeCopyMorphToolsSingleton* GetRuntimeCopyMorphToolsSingleton();

	UFUNCTION()
		static UMorphTarget* GetMorphByName(TArray<UMorphTarget*>& Morphs, FName Name);

private:

	static uint32 renameint;

	

	
	/*
		static void AsyncCopyMorphs(TArray<FCloneLODRenderData> TargetCloneLODRenderDatas, TArray<FCloneLODIndices> TargetCloneLODsIndices, 
			TArray<FCloneMorph> CopyFromCloneMorphs, TArray<FCloneLODRenderData> CopyFromCloneLODRenderDatas, TArray<FCloneLODIndices> CopyFromCloneLODsIndices,
			USkeletalMesh* TargetMesh, int32 SmoothCount, int32 AdditionalMultiplyCount, float Threshold, float SameVerticesThreshold,
			TArray<FString> MorphNamesToRemove, TMap<FString, UMorphTarget*> NewMorphTargetsMap, FDelegate Delegate, bool Overwrite);
			*/

};
