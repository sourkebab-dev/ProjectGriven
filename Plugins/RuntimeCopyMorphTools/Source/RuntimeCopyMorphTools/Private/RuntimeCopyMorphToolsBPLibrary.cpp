// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
// Copyright 2020 LunaticX, All Rights Reserved.

#include "RuntimeCopyMorphToolsBPLibrary.h"
#include "RuntimeCopyMorphTools.h"
#include "Engine/SkeletalMesh.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/MorphTarget.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/MultiSizeIndexContainer.h"
#include "Math/Vector.h"
#include "RenderingThread.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "TimerManager.h"
#include "Async/TaskGraphInterfaces.h"








void UInitMorphsAndResourcesUtility::InitMorphsAndResources()
{
	//UE_LOG(LogTemp, Warning, TEXT("testInitMorphsAndResources"));
	
	//FSuspendRenderingThread suspend = FSuspendRenderingThread(true);
	UE_LOG(LogTemp, Warning, TEXT("StartInitMorphsAndResources"));


	if (IsValid(TargetMesh))
	{
		
		for (auto& MorphNameToRemove : MorphNamesToRemove)
		{
			//UE_LOG(LogTemp, Warning, TEXT("tryToRemoveMorph"));
			UMorphTarget* MorphToRemove = URuntimeCopyMorphToolsBPLibrary::GetMorphByName(TargetMesh->MorphTargets, (FName)*MorphNameToRemove);
			if (IsValid(MorphToRemove))
			{
				//UE_LOG(LogTemp, Warning, TEXT("RemoveMorph"));
				MorphToRemove->Rename(*URuntimeCopyMorphToolsBPLibrary::GetRenameString(), TargetMesh);
				MorphToRemove->MarkPendingKill();
				TargetMesh->MorphTargets.RemoveSingleSwap(MorphToRemove);
			}
		}

		for (auto& Pair : MorphNameAndMorphTargetMap)
		{
			//UE_LOG(LogTemp, Warning, TEXT("tryToAddMorph"));
			//UE_LOG(LogTemp, Warning, TEXT("%s"), *Pair.Key);
			UObject* SameNameObject = FindObjectFast<UObject>(TargetMesh, (FName)*Pair.Key);

			if (!IsValid(SameNameObject))
			{
				UE_LOG(LogTemp, Warning, TEXT("ngentE"));
				UE_LOG(LogTemp, Warning, TEXT("AddUmorph"));
				Pair.Value->Rename(*Pair.Key, TargetMesh);
				TargetMesh->MorphTargets.Add(Pair.Value);
			}
			else
			{
				//if (IsValid(Cast<UMorphTarget>(SameNameObject)))
				//{
					UE_LOG(LogTemp, Warning, TEXT("RemovedSameNameUMorph"));
					SameNameObject->Rename(*URuntimeCopyMorphToolsBPLibrary::GetRenameString(), TargetMesh);
					SameNameObject->MarkPendingKill();
					Pair.Value->Rename(*Pair.Key, TargetMesh);
					TargetMesh->MorphTargets.Add(Pair.Value);
				//}
			}
		}
		

		FlushRenderingCommands();
		TargetMesh->ReleaseResources();
		FlushRenderingCommands();
		TargetMesh->InitMorphTargets();
		FlushRenderingCommands();
		TargetMesh->InitResources();

		UE_LOG(LogTemp, Warning, TEXT("XXXeeraaKONTOLAE"));


		//TargetMesh->InvalidateDeriveDataCacheGUID();
		//TargetMesh->SetLODImportedDataVersions(0, ESkeletalMeshGeoImportVersions::SkeletalMeshBuildRefactor, ESkeletalMeshSkinningImportVersions::SkeletalMeshBuildRefactor);
		
	}
	//suspend.~FSuspendRenderingThread();
	UE_LOG(LogTemp, Warning, TEXT("FinishInitMorphsAndResources"));
	
	this->RemoveFromRoot();
	this->MarkPendingKill();
	Delegate.ExecuteIfBound();
}



void UAsyncTransferMorphsUtility::AsyncTransferMorphs()
{
	TMap<FString, UMorphTarget*> NewMorphNameAndMorphTargetMap;

	for (auto& CloneMorph: CloneMorphs)
	{
		UMorphTarget* NewMorph = NewObject<UMorphTarget>();
		NewMorph->BaseSkelMesh = TargetMesh;
		for (auto& CloneMorphLOD: CloneMorph.CloneMorphLODModels)
		{
			FMorphTargetLODModel NewMorphLOD;
			NewMorphLOD.bGeneratedByEngine = false;
			NewMorphLOD.NumBaseMeshVerts = CloneMorphLOD.NumMeshVerts;
			NewMorphLOD.SectionIndices = CloneMorphLOD.SectionIndices;
			NewMorphLOD.Vertices = CloneMorphLOD.Vertices;

			NewMorph->MorphLODModels.Add(NewMorphLOD);
		}
		
		NewMorphNameAndMorphTargetMap.Add(CloneMorph.MorphName, NewMorph);
	}

	UInitMorphsAndResourcesUtility* InitMorphsAndResourcesUtility = NewObject<UInitMorphsAndResourcesUtility>();
	InitMorphsAndResourcesUtility->AddToRoot();

	InitMorphsAndResourcesUtility->TargetMesh = TargetMesh;
	InitMorphsAndResourcesUtility->MorphNameAndMorphTargetMap = NewMorphNameAndMorphTargetMap;
	InitMorphsAndResourcesUtility->MorphNamesToRemove = MorphNamesToRemove;
	InitMorphsAndResourcesUtility->Delegate = Delegate;

	TFunction<void()> _TaskFunc = [InitMorphsAndResourcesUtility]() {
		InitMorphsAndResourcesUtility->InitMorphsAndResources();
	};
	ENamedThreads::Type _Thread = ENamedThreads::GameThread;
	FFunctionGraphTask::CreateAndDispatchWhenReady(_TaskFunc, TStatId(), nullptr, _Thread);

	this->RemoveFromRoot();
	this->MarkPendingKill();
}



void UAsyncSaveMorphsUtility::AsyncSaveMorphs()
{
	FString JsonFullPath = FPaths::ProjectDir().Append(FileName);

	FSaveMorphs SaveMorphs;

	for (auto& MorphTarget : CloneMorphs)
	{

		FSaveMorph SaveMorph;

		SaveMorph.MorphName = MorphTarget.MorphName;
		for (uint32 MorphLODIndex = 0; MorphLODIndex < (uint32)MorphTarget.CloneMorphLODModels.Num(); MorphLODIndex++)
		{
			FSaveMorphLODModel NewSaveMorphLOD;
			NewSaveMorphLOD.NumMeshVerts = MorphTarget.CloneMorphLODModels[MorphLODIndex].NumMeshVerts;

			for (auto& IndicesIndex : MorphTarget.CloneMorphLODModels[MorphLODIndex].SectionIndices)
			{
				NewSaveMorphLOD.SectionIndices.Add(IndicesIndex);
			}

			for (auto& Vertex : MorphTarget.CloneMorphLODModels[MorphLODIndex].Vertices)
			{
				FSaveMorphDelta NewSaveMorphDelta = { Vertex.PositionDelta, Vertex.SourceIdx, Vertex.TangentZDelta };

				NewSaveMorphLOD.Vertices.Add(NewSaveMorphDelta);
			}

			SaveMorph.SaveMorphLODModels.Add(NewSaveMorphLOD);
		}

		SaveMorphs.Morphs.Add(SaveMorph);
	}
	
	FString SaveString;

	FJsonObjectConverter::UStructToJsonObjectString(FSaveMorphs::StaticStruct(), &SaveMorphs, SaveString, 0, 0);
	FFileHelper::SaveStringToFile(SaveString, *JsonFullPath);
	
	UInitMorphsAndResourcesUtility* InitMorphsAndResourcesUtility = NewObject<UInitMorphsAndResourcesUtility>();
	InitMorphsAndResourcesUtility->AddToRoot();

	InitMorphsAndResourcesUtility->Delegate = Delegate;

	TFunction<void()> _TaskFunc = [InitMorphsAndResourcesUtility]() {
		InitMorphsAndResourcesUtility->InitMorphsAndResources();
	};
	ENamedThreads::Type _Thread = ENamedThreads::GameThread;
	FFunctionGraphTask::CreateAndDispatchWhenReady(_TaskFunc, TStatId(), nullptr, _Thread);

	this->RemoveFromRoot();
	this->MarkPendingKill();
}



void UAsyncLoadMorphsUtility::AsyncLoadMorphs()
{
	TMap<FString, UMorphTarget*> NewMorphNameAndMorphTargetMap;
	FString JsonFullPath = FPaths::ProjectDir().Append(FileName);
	FString LoadString;
	FSaveMorphs LoadMorphs;

	FFileHelper::LoadFileToString(LoadString, *JsonFullPath);

	if (FJsonObjectConverter::JsonObjectStringToUStruct(LoadString, &LoadMorphs, 0, 0))
	{
		for (auto& LoadMorph : LoadMorphs.Morphs)
		{
			UMorphTarget* NewMorph = NewObject<UMorphTarget>();

			for (auto& SaveMorphLODModel : LoadMorph.SaveMorphLODModels)
			{
				FMorphTargetLODModel NewLODModel;
				NewLODModel.bGeneratedByEngine = false;
				NewLODModel.NumBaseMeshVerts = SaveMorphLODModel.NumMeshVerts;
				NewLODModel.SectionIndices = SaveMorphLODModel.SectionIndices;
				for (auto& MorphDelta : SaveMorphLODModel.Vertices)
				{
					FMorphTargetDelta NewDelta = { MorphDelta.PositionDelta, MorphDelta.TangentZDelta, MorphDelta.SourceIdx };
					NewLODModel.Vertices.Add(NewDelta);
				}
				NewMorph->MorphLODModels.Add(NewLODModel);
			}
			NewMorphNameAndMorphTargetMap.Add(LoadMorph.MorphName, NewMorph);
		}
	}

	TArray<FString> NewMorphMapKeys;
	NewMorphNameAndMorphTargetMap.GetKeys(NewMorphMapKeys);
	TArray<FString> MorphNamesToRemove;
	//Find same name morphs
	for (auto& CloneMorphName : CloneMorphNames)
	{
		for (auto& MorphName : NewMorphMapKeys)
		{
			if (CloneMorphName == MorphName)
			{
				if (Overwrite)
				{
					MorphNamesToRemove.Add(MorphName);
				}
				else 
				{
					NewMorphNameAndMorphTargetMap.Remove(MorphName);
				}
			}
		}
	}

	UInitMorphsAndResourcesUtility* InitMorphsAndResourcesUtility = NewObject<UInitMorphsAndResourcesUtility>();
	InitMorphsAndResourcesUtility->AddToRoot();

	InitMorphsAndResourcesUtility->MorphNamesToRemove = MorphNamesToRemove;
	InitMorphsAndResourcesUtility->MorphNameAndMorphTargetMap = NewMorphNameAndMorphTargetMap;
	InitMorphsAndResourcesUtility->TargetMesh = TargetMesh;
	InitMorphsAndResourcesUtility->Delegate = Delegate;

	TFunction<void()> _TaskFunc = [InitMorphsAndResourcesUtility]() {
		InitMorphsAndResourcesUtility->InitMorphsAndResources();
	};
	ENamedThreads::Type _Thread = ENamedThreads::GameThread;
	FFunctionGraphTask::CreateAndDispatchWhenReady(_TaskFunc, TStatId(), nullptr, _Thread);

	this->RemoveFromRoot();
	this->MarkPendingKill();
}



void UAsyncBakeMorphsUtility::AsyncBakeMorphs()
{
	UMorphTarget* NewMorph = NewObject<UMorphTarget>();

	for (int32 LODIndex = 0; LODIndex < CloneLODRenderDatas.Num(); LODIndex++)
	{
		FMorphTargetLODModel NewLODMorph;
		NewLODMorph.bGeneratedByEngine = false;
		NewLODMorph.NumBaseMeshVerts = CloneLODRenderDatas[LODIndex].Vertices.Num();//(int32)TargetLODRenderData.GetNumVertices();

		for (int32 i = 0; i < CloneLODRenderDatas[LODIndex].RenderSectionsNum; i++)
		{
			NewLODMorph.SectionIndices.Add(i);
		}

		TArray<FVector> NewDeltaBuffers;
		NewDeltaBuffers.Init({ 0, 0, 0 }, NewLODMorph.NumBaseMeshVerts);

		for (auto& CloneMorph: CloneMorphs)
		{

			
			
			
			if (!CloneMorph.CloneMorphLODModels.IsValidIndex(LODIndex))continue;
			
			
			

			for (auto& LODVertex: CloneMorph.CloneMorphLODModels[LODIndex].Vertices)
			{
				NewDeltaBuffers[LODVertex.SourceIdx] += LODVertex.PositionDelta * CloneMorph.MorphWeight;
			}
		}

		for (int32 NewDeltaBufferIndex = 0; NewDeltaBufferIndex < NewDeltaBuffers.Num(); NewDeltaBufferIndex++)
		{
			if (NewDeltaBuffers[NewDeltaBufferIndex].Size() > 0.000000001)
			{
				FMorphTargetDelta NewDelta;
				NewDelta.PositionDelta = NewDeltaBuffers[NewDeltaBufferIndex];
				NewDelta.SourceIdx = NewDeltaBufferIndex;
				NewDelta.TangentZDelta = CloneLODRenderDatas[LODIndex].Vertices[NewDeltaBufferIndex].TangentZ;

				NewLODMorph.Vertices.Add(NewDelta);
			}
		}

		NewMorph->MorphLODModels.Add(NewLODMorph);

	}

	UInitMorphsAndResourcesUtility* InitMorphsAndResourcesUtility = NewObject<UInitMorphsAndResourcesUtility>();
	InitMorphsAndResourcesUtility->AddToRoot();

	InitMorphsAndResourcesUtility->MorphNamesToRemove.Add(NewMorphName);
	InitMorphsAndResourcesUtility->MorphNameAndMorphTargetMap.Add(NewMorphName, NewMorph);
	InitMorphsAndResourcesUtility->TargetMesh = TargetMesh;
	InitMorphsAndResourcesUtility->Delegate = Delegate;

	TFunction<void()> _TaskFunc = [InitMorphsAndResourcesUtility]() {
		InitMorphsAndResourcesUtility->InitMorphsAndResources();
	};
	ENamedThreads::Type _Thread = ENamedThreads::GameThread;
	FFunctionGraphTask::CreateAndDispatchWhenReady(_TaskFunc, TStatId(), nullptr, _Thread);

	this->RemoveFromRoot();
	this->MarkPendingKill();
}



void UAsyncCopyMorphsUtility::AsyncCopyMorphs()
{
	UE_LOG(LogTemp, Warning, TEXT("StartCreateMorphs"));

	float SmoothStrength = 0.9;
	int32 ExtraDeltaCount = AdditionalMultiplyCount;

	//TMap<FString, UMorphTarget*> NewMorphTargetsMap;

	uint32 LODNumber = (uint32)FMath::Min<int32>(TargetCloneLODRenderDatas.Num(), CopyFromCloneLODRenderDatas.Num());


	//UE_LOG(LogTemp, Warning, TEXT("LODNumber%d"), LODNumber);

	TArray<FString> TargetMorphNames;
	for (auto& TargetCloneMorph : CopyFromCloneMorphs)
	{
		TargetMorphNames.Add(TargetCloneMorph.MorphName);
	}

	for (uint32 LODIndex = 0; LODIndex < LODNumber; LODIndex++)
	{
		//UE_LOG(LogTemp, Warning, TEXT("LOD%dstart"), LODIndex);
		if (LODIndex >= 1)break;
		
		UMorphTarget* NewMorph;

		TArray<FVertexWeight> VertexWeights;
		

		TArray<FSameVerticesSet> TargetMeshSameVerticesSetArr;
		TMap<uint32, uint32> TargetToBaseMeshSameVertexMap;

		if (!VertexWeightMapFileName.IsEmpty())
		{
			FString JsonFullPath = FPaths::ProjectDir().Append(VertexWeightMapFileName);
			FString LoadString;
			FVertexWeightMap VertexWeightMapLoad;

			FFileHelper::LoadFileToString(LoadString, *JsonFullPath);

			if (FJsonObjectConverter::JsonObjectStringToUStruct(LoadString, &VertexWeightMapLoad, 0, 0))
			{
				UE_LOG(LogTemp, Warning, TEXT("VertexWeightMapLoaded"));
				VertexWeights = VertexWeightMapLoad.VertexWeights;
				TargetMeshSameVerticesSetArr = VertexWeightMapLoad.TargetMeshSameVerticesSetArr;
				TargetToBaseMeshSameVertexMap = VertexWeightMapLoad.TargetToBaseMeshSameVertexMap;
			}
		}

		

		for (auto& MorphName : TargetMorphNames)
		{

			if (LODIndex == 0)
			{
				NewMorph = NewObject<UMorphTarget>();
				NewMorph->BaseSkelMesh = TargetMesh;
			}
			else {
				if (NewMorphTargetsMap.Contains(MorphName))
				{
					UE_LOG(LogTemp, Warning, TEXT("FoundSameNameMorph"));
					NewMorph = *NewMorphTargetsMap.Find(MorphName);
				}
				else {
					continue;
				}

			}


			bool CloneMorphLODsHaveValidMorphLOD = false;
			for (auto& CopyFromCloneMorph : CopyFromCloneMorphs)
			{
				if (CopyFromCloneMorph.MorphName == MorphName)
				{
					if (CopyFromCloneMorph.CloneMorphLODModels.IsValidIndex(LODIndex))
					{
						CloneMorphLODsHaveValidMorphLOD = true;
						break;
					}
					else
					{
						CloneMorphLODsHaveValidMorphLOD = false;
						break;
					}
				}
			}
			if (!CloneMorphLODsHaveValidMorphLOD)continue;


			if (!VertexWeights.IsValidIndex(0))
			{
				//UE_LOG(LogTemp, Warning, TEXT("StartCalculationVertexWeightMap"));
				for (uint32 TargetIndex = 0; TargetIndex < (uint32)TargetCloneLODRenderDatas[LODIndex].Vertices.Num()/*TargetLODRenderData.GetNumVertices()*/; TargetIndex++)
				{
					//UE_LOG(LogTemp, Warning, TEXT("targetclonelodrenderdataverticesnum %d"), TargetCloneLODRenderDatas[LODIndex].Vertices.Num());
					FVector TargetVertexPosition = TargetCloneLODRenderDatas[LODIndex].Vertices[TargetIndex].VertexPosition;//TargetLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(TargetIndex);
					TArray<uint32> CloseTargetVertexIndices;
					for (uint32 SecondTargetIndex = TargetIndex; SecondTargetIndex < (uint32)TargetCloneLODRenderDatas[LODIndex].Vertices.Num(); SecondTargetIndex++)
					{
						FVector SecondTargetVertexPosition = TargetCloneLODRenderDatas[LODIndex].Vertices[SecondTargetIndex].VertexPosition;//TargetLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(SecondTargetIndex);
						if ((TargetVertexPosition.Y - 0.001 < SecondTargetVertexPosition.Y && SecondTargetVertexPosition.Y < TargetVertexPosition.Y + 0.001)
							&& (TargetVertexPosition.Z - 0.001 < SecondTargetVertexPosition.Z && SecondTargetVertexPosition.Z < TargetVertexPosition.Z + 0.001)
							&& (TargetVertexPosition.X - 0.001 < SecondTargetVertexPosition.X && SecondTargetVertexPosition.X < TargetVertexPosition.X + 0.001))
						{
							bool FoundVerticesSet = false;
							for (auto& VerticesSet : TargetMeshSameVerticesSetArr)
							{
								if (VerticesSet.SameVertices.Contains(TargetIndex) || VerticesSet.SameVertices.Contains(SecondTargetIndex))
								{
									VerticesSet.SameVertices.Add(TargetIndex);
									VerticesSet.SameVertices.Add(SecondTargetIndex);
									FoundVerticesSet = true;
									break;
								}
							}
							if (!FoundVerticesSet)
							{
								TargetMeshSameVerticesSetArr.Add(FSameVerticesSet({ {TargetIndex, SecondTargetIndex} }));
							}
						}
					}

				}








				//UE_LOG(LogTemp, Warning, TEXT("NoValidVertexWeights"));
				//make vertex weight map
				for (uint32 TargetIndex = 0; TargetIndex < (uint32)TargetCloneLODRenderDatas[LODIndex].Vertices.Num()/*TargetLODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.GetNumVertices()*/; TargetIndex++)
				{
					bool ContinueBool = 0;

					FVector TargetVertexPosition = TargetCloneLODRenderDatas[LODIndex].Vertices[TargetIndex].VertexPosition;//TargetLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(TargetIndex);
					TArray<uint32> CloseBaseVertexIndices;
					//add close vertices to array
					for (uint32 BaseVertexIndex = 0; BaseVertexIndex < (uint32)CopyFromCloneLODRenderDatas[LODIndex].Vertices.Num()/*BaseLODRenderData.GetNumVertices()*/; BaseVertexIndex++)
					{
						FVector BaseVertexPosition = CopyFromCloneLODRenderDatas[LODIndex].Vertices[BaseVertexIndex].VertexPosition;//BaseLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(BaseVertexIndex);

						if ((TargetVertexPosition.Y - Threshold < BaseVertexPosition.Y && BaseVertexPosition.Y < TargetVertexPosition.Y + Threshold)
							&& (TargetVertexPosition.Z - Threshold < BaseVertexPosition.Z && BaseVertexPosition.Z < TargetVertexPosition.Z + Threshold)
							&& (TargetVertexPosition.X - Threshold < BaseVertexPosition.X && BaseVertexPosition.X < TargetVertexPosition.X + Threshold))
						{
							CloseBaseVertexIndices.Add(BaseVertexIndex);
						}
					}


					//add false vertexweight and continue if closevertexindices have no element
					if (CloseBaseVertexIndices.Num() == 0)
					{
						VertexWeights.Add(FVertexWeight{ false,0,0,0,0,0,0 });

						continue;
					}





					float VerticesGap = Threshold * 3;
					uint32 ClosestVertexIndex = 0;
					//find same or closest vertex
					for (uint32 BaseVertexIndex : CloseBaseVertexIndices)
					{
						FVector BaseVertexPosition = CopyFromCloneLODRenderDatas[LODIndex].Vertices[BaseVertexIndex].VertexPosition;//BaseLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(BaseVertexIndex);
						float NewVerticesGap = (TargetVertexPosition - BaseVertexPosition).Size();

						//same vertex
						if (NewVerticesGap < SameVerticesThreshold)
						{
							VertexWeights.Add(FVertexWeight{ true,(int32)TargetIndex,1,0,0,0,0 });
							TargetToBaseMeshSameVertexMap.Add(TargetIndex, BaseVertexIndex);

							ContinueBool = 1;
							break;
						}
						else if (NewVerticesGap < VerticesGap)
						{
							VerticesGap = NewVerticesGap;
							ClosestVertexIndex = BaseVertexIndex;
						}

					}



					//continue if there is same vertex
					if (ContinueBool)
					{
						continue;
					}





					//find closest face



					//find closest side




					//float vertextestweight = 1 - (TargetVertexPosition - BaseLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(ClosestVertexIndex)).Size() / Threshold;
					//only closest vertex exist
					VertexWeights.Add(FVertexWeight{ true,(int32)ClosestVertexIndex,1.0,0,0,0,0 });
					//UE_LOG(LogTemp, Warning, TEXT("VertexIndex%d"), ClosestVertexIndex);

					continue;

				}


				if (!VertexWeightMapFileName.IsEmpty())
				{
					FString JsonFullPath = FPaths::ProjectDir().Append(VertexWeightMapFileName);
					
					FVertexWeightMap VertexWeightMapSave;

					VertexWeightMapSave.VertexWeights = VertexWeights;
					VertexWeightMapSave.TargetMeshSameVerticesSetArr = TargetMeshSameVerticesSetArr;
					VertexWeightMapSave.TargetToBaseMeshSameVertexMap = TargetToBaseMeshSameVertexMap;

					FString SaveString;
					FJsonObjectConverter::UStructToJsonObjectString(FVertexWeightMap::StaticStruct(), &VertexWeightMapSave, SaveString, 0, 0);
					FFileHelper::SaveStringToFile(SaveString, *JsonFullPath);
				}

			}



			//make new morph map


			FMorphTargetLODModel NewLODMorph;
			NewLODMorph.bGeneratedByEngine = false;
			NewLODMorph.NumBaseMeshVerts = TargetCloneLODRenderDatas[LODIndex].Vertices.Num();//TargetLODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.GetNumVertices();
			for (int32 i = 0; i < TargetCloneLODRenderDatas[LODIndex].RenderSectionsNum/*TargetLODRenderData.RenderSections.Num()*/; i++)
			{
				NewLODMorph.SectionIndices.Add(i);
			}

			//CopyFromCloneMorphs[0].CloneMorphLODModels[LODIndex].

			uint32 CopyFromMeshMorphIndex = 0;
			bool CopyFromCloneMorphsHaveMorphName = false;
			for (uint32 CopyFromCloneMorphIndex = 0; CopyFromCloneMorphIndex < (uint32)CopyFromCloneMorphs.Num(); CopyFromCloneMorphIndex++)
			{
				if (CopyFromCloneMorphs[CopyFromCloneMorphIndex].MorphName == MorphName)
				{
					CopyFromCloneMorphsHaveMorphName = true;
					CopyFromMeshMorphIndex = CopyFromCloneMorphIndex;
					break;
				}
			}
			if (!CopyFromCloneMorphsHaveMorphName)continue;
			//if (!BaseMesh->FindMorphTarget((FName)*MorphName)->MorphLODModels.IsValidIndex(LODIndex)) { continue; }

			//FMorphTargetLODModel& BaseMeshMorph = BaseMesh->FindMorphTarget((FName)*MorphName)->MorphLODModels[LODIndex];

			for (uint32 index = 0; index < (uint32)NewLODMorph.NumBaseMeshVerts; index++)
			{
				//UE_LOG(LogTemp, Warning, TEXT("vertexindexandweight %d,%f"),VertexWeights[index].FirstVertexIndex,VertexWeights[index].FirstVertexWeight);
				if (VertexWeights[index].HaveAnyWeight == false) { continue; }

				FMorphTargetDelta NewDelta;
				NewDelta.PositionDelta = FVector{ 0,0,0 };
				NewDelta.SourceIdx = index;
				//NewDelta.PositionDelta = FVector(100,0,0);

				bool BasemeshMorphHaveValidVertex = false;
				for (uint32 BasemeshMorphVertexIndex = 0; BasemeshMorphVertexIndex
					< (uint32)CopyFromCloneMorphs[CopyFromMeshMorphIndex].CloneMorphLODModels[LODIndex].Vertices.Num()
					/*(uint32)BaseMeshMorph.Vertices.Num()*/; BasemeshMorphVertexIndex++)
				{


					if (CopyFromCloneMorphs[CopyFromMeshMorphIndex].CloneMorphLODModels[LODIndex].Vertices[BasemeshMorphVertexIndex].SourceIdx
						/*BaseMeshMorph.Vertices[BasemeshMorphVertexIndex].SourceIdx*/ == VertexWeights[index].FirstVertexIndex)
					{
						NewDelta.PositionDelta += CopyFromCloneMorphs[CopyFromMeshMorphIndex].CloneMorphLODModels[LODIndex].Vertices[BasemeshMorphVertexIndex].PositionDelta
							* VertexWeights[index].FirstVertexWeight;
						//NewDelta.TangentZDelta += /*BaseMeshMorph.Vertices[BasemeshMorphVertexIndex].TangentZDelta * VertexWeights[index].FirstVertexWeight +*/ BaseLODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(VertexWeights[index].FirstVertexIndex);
						//if(VertexWeights[index].FirstVertexWeight < 1)UE_LOG(LogTemp, Warning, TEXT("WeightOver1"));
						BasemeshMorphHaveValidVertex = true;
					}
					/*
					if (BaseMeshMorph.Vertices[BasemeshMorphVertexIndex].SourceIdx == VertexWeights[index].SecondVertexIndex)
					{
						NewDelta.PositionDelta += BaseMeshMorph.Vertices[BasemeshMorphVertexIndex].PositionDelta * VertexWeights[index].SecondVertexWeight;

						BasemeshMorphHaveValidVertex = true;
					}

					if (BaseMeshMorph.Vertices[BasemeshMorphVertexIndex].SourceIdx == VertexWeights[index].ThirdVertexIndex)
					{
						NewDelta.PositionDelta += BaseMeshMorph.Vertices[BasemeshMorphVertexIndex].PositionDelta * VertexWeights[index].ThirdVertexWeight;

						BasemeshMorphHaveValidVertex = true;
					}*/
				}

				if (!BasemeshMorphHaveValidVertex) { continue; }

				//UE_LOG(LogTemp, Warning, TEXT("newdelta %f,%f,%f"), NewDelta.PositionDelta.X, NewDelta.PositionDelta.Y, NewDelta.PositionDelta.Z);

				NewLODMorph.Vertices.Add(NewDelta);
			}



			//smooth deltas

			for (int smoothnumber = 0; smoothnumber < SmoothCount; smoothnumber++)
			{

				TArray<FVector> NewDeltasArr;
				for (uint32 i = 0; i < (uint32)TargetCloneLODRenderDatas[LODIndex].Vertices.Num(); i++)
				{
					NewDeltasArr.Add(FVector(0, 0, 0));
				}

				for (FMorphTargetDelta& delta : NewLODMorph.Vertices)
				{
					NewDeltasArr[delta.SourceIdx] = delta.PositionDelta;
				}

				TArray<TArray<FVector>> TempSmoothedDeltasArr;
				for (uint32 i = 0; i < (uint32)TargetCloneLODRenderDatas[LODIndex].Vertices.Num(); i++)
				{
					TempSmoothedDeltasArr.Add(TArray<FVector>());
				}

				for (auto& delta : NewLODMorph.Vertices)
				{
					//copydeltas.Add(delta);
					for (uint32 TargetLODindex = 0; TargetLODindex < (uint32)TargetCloneLODsIndices[LODIndex].LODIndices.Num(); TargetLODindex += 3)
					{
						if (delta.SourceIdx == TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex]
							|| delta.SourceIdx == TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 1]
							|| delta.SourceIdx == TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 2])
						{
							FVector FirstVertexDelta = ((NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 1]]
								+ NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 2]]) / 2
								- NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex]]) * SmoothStrength + NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex]];

							FVector SecondVertexDelta = ((NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 2]]
								+ NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex]]) / 2
								- NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 1]]) * SmoothStrength + NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 1]];

							FVector ThirdVertexDelta = ((NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex]]
								+ NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 1]]) / 2
								- NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 2]]) * SmoothStrength + NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 2]];


							TempSmoothedDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex]].Add(FirstVertexDelta);

							TempSmoothedDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 1]].Add(SecondVertexDelta);

							TempSmoothedDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 2]].Add(ThirdVertexDelta);




						}
					}
				}





				NewLODMorph.Vertices.Empty();
				for (uint32 i = 0; i < (uint32)TempSmoothedDeltasArr.Num(); i++)
				{
					if (TempSmoothedDeltasArr[i].Num() > 0)
					{
						FMorphTargetDelta NewDelta;
						NewDelta.SourceIdx = i;
						NewDelta.PositionDelta = FVector(0, 0, 0);

						for (FVector tempvector : TempSmoothedDeltasArr[i])
						{
							NewDelta.PositionDelta += tempvector;
						}

						NewDelta.PositionDelta /= TempSmoothedDeltasArr[i].Num();

						NewLODMorph.Vertices.Add(NewDelta);
					}
				}

			}

			/*
			//add extra delta to target mesh vertex deltas to keep original relative position
			for (int32 index = 0; index < ExtraDeltaCount; index++)
			{
				for (uint32 TargetIndex = 0; TargetIndex < (uint32)NewLODMorph.Vertices.Num(); TargetIndex++)
				{

					uint32 TargetLODVertexIndex = NewLODMorph.Vertices[TargetIndex].SourceIdx;
					FVector TargetMorphDelta = NewLODMorph.Vertices[TargetIndex].PositionDelta;//VertexWeights[TargetIndex].FirstVertexWeight * BaseMeshMorph.Vertices[VertexWeights[TargetIndex].FirstVertexIndex].PositionDelta
							//+ VertexWeights[TargetIndex].SecondVertexWeight * BaseMeshMorph.Vertices[VertexWeights[TargetIndex].SecondVertexIndex].PositionDelta
							//+ VertexWeights[TargetIndex].ThirdVertexWeight * BaseMeshMorph.Vertices[VertexWeights[TargetIndex].ThirdVertexIndex].PositionDelta;
					FVector TargetLODVertexPosition = TargetCloneLODRenderDatas[LODIndex].Vertices[TargetLODVertexIndex].VertexPosition;//TargetLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(TargetLODVertexIndex)
						+ TargetMorphDelta;

					FVector TargetLODVertexOriginalRelativePosition = (TargetLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(TargetLODVertexIndex)
						- BaseLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexWeights[TargetLODVertexIndex].FirstVertexIndex)) * VertexWeights[TargetLODVertexIndex].FirstVertexWeight
						+ (TargetLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(TargetLODVertexIndex)
							- BaseLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexWeights[TargetLODVertexIndex].SecondVertexIndex)) * VertexWeights[TargetLODVertexIndex].SecondVertexWeight
						+ (TargetLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(TargetLODVertexIndex)
							- BaseLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexWeights[TargetLODVertexIndex].ThirdVertexIndex)) * VertexWeights[TargetLODVertexIndex].ThirdVertexWeight;


					FVector BaseMeshMorphFirstDelta;
					FVector BaseMeshMorphSecondDelta;
					FVector BaseMeshMorphThirdDelta;
					for (uint32 BaseMeshMorphIndex = 0; BaseMeshMorphIndex < (uint32)BaseMeshMorph.Vertices.Num(); BaseMeshMorphIndex++)
					{
						if (BaseMeshMorph.Vertices[BaseMeshMorphIndex].SourceIdx == VertexWeights[TargetLODVertexIndex].FirstVertexIndex)
						{
							BaseMeshMorphFirstDelta = BaseMeshMorph.Vertices[BaseMeshMorphIndex].PositionDelta;
						}

						if (BaseMeshMorph.Vertices[BaseMeshMorphIndex].SourceIdx == VertexWeights[TargetLODVertexIndex].SecondVertexIndex)
						{
							BaseMeshMorphSecondDelta = BaseMeshMorph.Vertices[BaseMeshMorphIndex].PositionDelta;
						}

						if (BaseMeshMorph.Vertices[BaseMeshMorphIndex].SourceIdx == VertexWeights[TargetLODVertexIndex].ThirdVertexIndex)
						{
							BaseMeshMorphThirdDelta = BaseMeshMorph.Vertices[BaseMeshMorphIndex].PositionDelta;
						}
					}

					FVector TargetLODVertexNewRelativePosition = (TargetLODVertexPosition
						- BaseLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexWeights[TargetLODVertexIndex].FirstVertexIndex) - BaseMeshMorphFirstDelta)
						* VertexWeights[TargetLODVertexIndex].FirstVertexWeight
						+ (TargetLODVertexPosition
							- BaseLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexWeights[TargetLODVertexIndex].SecondVertexIndex) - BaseMeshMorphSecondDelta)
						* VertexWeights[TargetLODVertexIndex].SecondVertexWeight
						+ (TargetLODVertexPosition
							- BaseLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexWeights[TargetLODVertexIndex].ThirdVertexIndex) - BaseMeshMorphThirdDelta)
						* VertexWeights[TargetLODVertexIndex].ThirdVertexWeight;



					if (VertexWeights[TargetLODVertexIndex].HaveAnyWeight)
					{

						if (FVector::DotProduct(TargetLODVertexPosition - BaseMeshMorphFirstDelta
							- BaseLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexWeights[TargetLODVertexIndex].FirstVertexIndex)
							, TargetMorphDelta) < 0
							|| (FVector::DotProduct(TargetLODVertexPosition - BaseMeshMorphSecondDelta
								- BaseLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexWeights[TargetLODVertexIndex].SecondVertexIndex)
								, TargetMorphDelta) < 0 && abs(VertexWeights[TargetLODVertexIndex].SecondVertexWeight) > 0.00001)
							|| (FVector::DotProduct(TargetLODVertexPosition - BaseMeshMorphThirdDelta
								- BaseLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexWeights[TargetLODVertexIndex].ThirdVertexIndex)
								, TargetMorphDelta) < 0 && abs(VertexWeights[TargetLODVertexIndex].ThirdVertexWeight) > 0.00001))
						{
							NewLODMorph.Vertices[TargetIndex].PositionDelta *= 1.1;
						}

						//if (FVector::DotProduct(TargetLODVertexOriginalRelativePosition, TargetLODVertexNewRelativePosition) < 0)
						//{
							//NewLODMorph.Vertices[TargetIndex].PositionDelta *= 1.1;
						//}



					}
				}
			}
			*/


			//fix targetmesh same vertices deltas
			for (FSameVerticesSet& SameVertices : TargetMeshSameVerticesSetArr)
			{
				FVector SumDeltas = { 0,0,0 };
				//UE_LOG(LogTemp, Warning, TEXT("sameverticesnum%d"), SameVertices.SameVertices.Num());

				for (uint32 TargetLODMorphIndex = 0; TargetLODMorphIndex < (uint32)NewLODMorph.Vertices.Num(); TargetLODMorphIndex++)
				{
					if (SameVertices.SameVertices.Contains(NewLODMorph.Vertices[TargetLODMorphIndex].SourceIdx))
					{
						SumDeltas += NewLODMorph.Vertices[TargetLODMorphIndex].PositionDelta;
					}
				}

				FVector AverageDelta = SumDeltas / SameVertices.SameVertices.Num();

				for (uint32 TargetLODMorphIndex = 0; TargetLODMorphIndex < (uint32)NewLODMorph.Vertices.Num(); TargetLODMorphIndex++)
				{
					if (SameVertices.SameVertices.Contains(NewLODMorph.Vertices[TargetLODMorphIndex].SourceIdx))
					{
						NewLODMorph.Vertices[TargetLODMorphIndex].PositionDelta = AverageDelta;
					}
				}


			}






			//fix same vertex delta
			for (FMorphTargetDelta& newdelta : NewLODMorph.Vertices)
			{
				//UE_LOG(LogTemp, Warning, TEXT("newdelta %f,%f,%f"), newdelta.PositionDelta.X, newdelta.PositionDelta.Y, newdelta.PositionDelta.Z);
				if (TargetToBaseMeshSameVertexMap.Contains(newdelta.SourceIdx))
				{
					newdelta.PositionDelta = FVector(0, 0, 0);


					for (FMorphTargetDelta& basedelta : CopyFromCloneMorphs[CopyFromMeshMorphIndex].CloneMorphLODModels[LODIndex].Vertices)
					{
						if (basedelta.SourceIdx == TargetToBaseMeshSameVertexMap[newdelta.SourceIdx])
						{
							newdelta.PositionDelta = basedelta.PositionDelta;

							break;
						}
					}

				}
				else
				{
					for (int32 i = 0; i < ExtraDeltaCount; i++)
					{
						newdelta.PositionDelta *= 1.1;
					}

					if (UniformInflateMagnitude > 0.00001)
					{
						FVector CloneVector = newdelta.PositionDelta;
						CloneVector.Normalize();
						newdelta.PositionDelta += CloneVector * UniformInflateMagnitude;
					}

				}
			}


			//fix tanZnormal
			for (FMorphTargetDelta& newdelta : NewLODMorph.Vertices)
			{
				//UE_LOG(LogTemp, Warning, TEXT("newdelta %f,%f,%f"), newdelta.PositionDelta.X, newdelta.PositionDelta.Y, newdelta.PositionDelta.Z);
				newdelta.TangentZDelta = TargetCloneLODRenderDatas[LODIndex].Vertices[newdelta.SourceIdx].TangentZ;//TargetLODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(newdelta.SourceIdx);
			}


			//UE_LOG(LogTemp, Warning, TEXT("LOD%dfinish"), LODIndex);

			//FString NewString = MorphName.ToString();
			NewMorph->MorphLODModels.Add(NewLODMorph);
			if (LODIndex == 0) { NewMorphTargetsMap.Add(MorphName, NewMorph); }

		}

	}


	UInitMorphsAndResourcesUtility* InitMorphsAndResourcesUtility = NewObject<UInitMorphsAndResourcesUtility>();
	InitMorphsAndResourcesUtility->AddToRoot();

	InitMorphsAndResourcesUtility->TargetMesh = TargetMesh;
	InitMorphsAndResourcesUtility->MorphNamesToRemove = MorphNamesToRemove;
	InitMorphsAndResourcesUtility->MorphNameAndMorphTargetMap = NewMorphTargetsMap;
	InitMorphsAndResourcesUtility->Delegate = Delegate;


	TFunction<void()> _TaskFunc = [InitMorphsAndResourcesUtility]() {
		InitMorphsAndResourcesUtility->InitMorphsAndResources();
	};
	ENamedThreads::Type _Thread = ENamedThreads::GameThread;
	FFunctionGraphTask::CreateAndDispatchWhenReady(_TaskFunc, TStatId(), nullptr, _Thread);


	//TargetMesh->InitResources();

	UE_LOG(LogTemp, Warning, TEXT("FinishCreateMorphs"));

}


/*
void UAsyncUniformInflateMorphsUtility::AsyncUniformInflateMorphs()
{

}
*/


URuntimeCopyMorphToolsBPLibrary::URuntimeCopyMorphToolsBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


/*
float URuntimeCopyMorphToolsBPLibrary::RuntimeCopyMorphToolsSampleFunction(float Param)
{
	return -1;
}
*/


uint32 URuntimeCopyMorphToolsBPLibrary::renameint = 0;



FString URuntimeCopyMorphToolsBPLibrary::GetRenameString()
{
	renameint++;
	if (renameint > 200000000)renameint = 0;
	FString RenameString = TEXT("RuntimeCopyMorphTools_OldMorphTargetPendingKill")
		+ FString::FromInt(renameint);
	return RenameString;
}


/*
void URuntimeCopyMorphToolsBPLibrary::debugfunc(USkeletalMesh* TargetMesh)
{
	FString objname = TEXT("asynctest");
	UObject* SameNameObject = FindObjectFast<UObject>(TargetMesh, (FName)*objname);
	if (!IsValid(SameNameObject))
	{
		UAsyncTest* asynctest = NewObject<UAsyncTest>();
		asynctest->Rename(*objname, TargetMesh);
		asynctest->AddToRoot();

		asynctest->TestLODRenderDataVertices.Add({ FVector(1,5,3),FVector(2,6,7) });
		asynctest->TestLODRenderDataVertices.Add({ FVector(1,5,3),FVector(2,6,7) });
		asynctest->TestLODRenderDataVertices.Add({ FVector(1,5,3),FVector(2,6,7) });

		UE_LOG(LogTemp, Warning, TEXT("NoAsyncTestObject AddNewOne"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FoundAsyncTestObject"));
		UE_LOG(LogTemp, Warning, TEXT("LODRenderSction %f"), Cast<UAsyncTest>(SameNameObject)->TestLODRenderDataVertices[2].VertexPosition.X);
	}

	
	UAsyncTest* asynctest = NewObject<UAsyncTest>(TargetMesh);
	asynctest->AddToRoot();

	//asynctest->AsyncTest();
	
	TFunction<void()> _TaskFunc = [asynctest]() {
		asynctest->AsyncTest();
	};
	ENamedThreads::Type _Thread = ENamedThreads::AnyBackgroundHiPriTask;

	asynctest->taskevent = FFunctionGraphTask::CreateAndDispatchWhenReady(_TaskFunc, TStatId(), nullptr, _Thread);

	FFunctionGraphTask::CreateAndDispatchWhenReady(_TaskFunc, TStatId(), nullptr, _Thread);
	
	
}
*/

/*
void UAsyncTest::BeginAsyncTest()
{
	FTimerManager& timemanager = GWorld->GetTimerManager();
	//FTimerHandle TimerHandle;
	timemanager.SetTimer(TimerHandle, this, &UAsyncTest::AsyncTest,1.0f, true);
	UE_LOG(LogTemp, Warning, TEXT("BeginAsyncTest"));

}



void UAsyncTest::AsyncTest()
{
	int32 i = 0;
}
*/


void URuntimeCopyMorphToolsBPLibrary::RemoveMorphs(USkeletalMesh* TargetMesh, TArray<FString> MorphNames)
{
	if (!IsValid(TargetMesh))return;
	TArray<UMorphTarget*> MorphsToRemove;

	for (auto& Morph: TargetMesh->MorphTargets)
	{
		for (auto& MorphName : MorphNames)
		{
			if (Morph->GetFName() == (FName)*MorphName)
			{
				Morph->Rename(*GetRenameString(), TargetMesh);
				MorphsToRemove.Add(Morph);
			}
		}
	}

	for (auto& Morph: MorphsToRemove)
	{
		Morph->MarkPendingKill();
		TargetMesh->MorphTargets.RemoveSingleSwap(Morph);
		//TargetMesh->UnregisterMorphTarget(Morph);
	}

	FlushRenderingCommands();
	TargetMesh->ReleaseResources();
	FlushRenderingCommands();
	TargetMesh->InitMorphTargets();
	FlushRenderingCommands();
	TargetMesh->InitResources();



#if WITH_EDITOR
	TargetMesh->InvalidateDeriveDataCacheGUID();
	TargetMesh->SetLODImportedDataVersions(0, ESkeletalMeshGeoImportVersions::SkeletalMeshBuildRefactor, ESkeletalMeshSkinningImportVersions::SkeletalMeshBuildRefactor);
#endif

}



void URuntimeCopyMorphToolsBPLibrary::RenameMorph(USkeletalMesh* TargetMesh, FString OldMorphName, FString NewMorphName)
{
	if (!IsValid(TargetMesh))return;
	if (OldMorphName == NewMorphName)return;
	
	TSet<FName> TargetMeshMorphNamesSet;
	for (auto& Morph : TargetMesh->MorphTargets) { TargetMeshMorphNamesSet.Add(Morph->GetFName()); }

	if (!TargetMeshMorphNamesSet.Contains((FName)*OldMorphName))return;


	if (TargetMeshMorphNamesSet.Contains((FName)*NewMorphName))
	{
		UMorphTarget* MorphToRemove = GetMorphByName(TargetMesh->MorphTargets, (FName)*NewMorphName);
		MorphToRemove->Rename(*GetRenameString(), TargetMesh);
		MorphToRemove->MarkPendingKill();
		TargetMesh->MorphTargets.RemoveSingleSwap(MorphToRemove);

		UMorphTarget* MorphToRename = GetMorphByName(TargetMesh->MorphTargets, (FName)*OldMorphName);
		MorphToRename->Rename(*NewMorphName, TargetMesh);
	}
	else
	{
		UObject* SameNameObject = FindObjectFast<UObject>(TargetMesh, (FName)*NewMorphName);

		UMorphTarget* MorphToRename = GetMorphByName(TargetMesh->MorphTargets, (FName)*OldMorphName);

		if (!IsValid(SameNameObject))
		{
			UE_LOG(LogTemp, Warning, TEXT("RenameUmorph"));
			MorphToRename->Rename(*NewMorphName, TargetMesh);
		}
		else
		{
			if (IsValid(Cast<UMorphTarget>(SameNameObject)))
			{
				UE_LOG(LogTemp, Warning, TEXT("RemoveSameNameUMorph"));
				SameNameObject->Rename(*URuntimeCopyMorphToolsBPLibrary::GetRenameString(), TargetMesh);
				SameNameObject->MarkPendingKill();
				MorphToRename->Rename(*NewMorphName, TargetMesh);
			}
		}

	}

	
	FlushRenderingCommands();
	TargetMesh->ReleaseResources();
	FlushRenderingCommands();
	TargetMesh->InitMorphTargets();
	FlushRenderingCommands();
	TargetMesh->InitResources();


#if WITH_EDITOR
	TargetMesh->InvalidateDeriveDataCacheGUID();
	TargetMesh->SetLODImportedDataVersions(0, ESkeletalMeshGeoImportVersions::SkeletalMeshBuildRefactor, ESkeletalMeshSkinningImportVersions::SkeletalMeshBuildRefactor);
#endif

}



void URuntimeCopyMorphToolsBPLibrary::TransferMorphs(USkeletalMesh* TargetMesh, USkeletalMesh* TransferFromMesh, TArray<FString> MorphNames, FDelegate Done, bool Overwrite)
{
	if (!IsValid(TargetMesh) || !IsValid(TransferFromMesh))return;

	UAsyncTransferMorphsUtility* AsyncTransferMorphsUtility = NewObject<UAsyncTransferMorphsUtility>();
	AsyncTransferMorphsUtility->AddToRoot();

	AsyncTransferMorphsUtility->TargetMesh = TargetMesh;
	AsyncTransferMorphsUtility->Delegate = Done;
	
	
	
	
	TSet<FName> TargetMeshMorphNamesSet;
	TSet<FName> TransferFromMeshMorphNamesSet;
	TSet<FName> MorphNamesSet;
	
	for (auto& Morph : TargetMesh->MorphTargets) { TargetMeshMorphNamesSet.Add(Morph->GetFName()); }
	for (auto& Morph : TransferFromMesh->MorphTargets) { TransferFromMeshMorphNamesSet.Add(Morph->GetFName()); }
	for (auto& MorphName : MorphNames) { MorphNamesSet.Add((FName)*MorphName); }
	
	TSet<FName> IntesectCopyFromAndMorph = TransferFromMeshMorphNamesSet.Intersect(MorphNamesSet);
	if (!(IntesectCopyFromAndMorph.Num() > 0))return;

	if(Overwrite)
	{
		TSet<FName> IntersectAll = TargetMeshMorphNamesSet.Intersect(IntesectCopyFromAndMorph);
		//for (auto& MorphName : IntersectAll)UE_LOG(LogTemp, Warning, TEXT("Intersect%s"), *MorphName.ToString());

		if (IntersectAll.Num() > 0)
		{
			for (auto& MorphName : IntersectAll)
			{
				/*
				UMorphTarget* MorphToRemove = GetMorphByName(TargetMesh->MorphTargets, MorphName);

				
				MorphToRemove->Rename(*GetRenameString(), TargetMesh);
				MorphToRemove->MarkPendingKill();
				TargetMesh->MorphTargets.RemoveSingleSwap(MorphToRemove);

				TargetMesh->ReleaseResources();
				TargetMesh->InitMorphTargets();
				TargetMesh->InitResources();
				*/
				AsyncTransferMorphsUtility->MorphNamesToRemove.Add(MorphName.ToString());

			}
			

			AsyncTransferMorphsUtility->MorphNames = IntesectCopyFromAndMorph;
			
		}
		else
		{
			AsyncTransferMorphsUtility->MorphNames = IntesectCopyFromAndMorph;
			
		}
	}
	else 
	{
		AsyncTransferMorphsUtility->MorphNames = IntesectCopyFromAndMorph.Difference(TargetMeshMorphNamesSet);
		
	}

	for (auto& Morph : TransferFromMesh->MorphTargets)
	{
		if (AsyncTransferMorphsUtility->MorphNames.Contains(Morph->GetFName()))
		{
			FCloneMorph NewCloneMorph;
			NewCloneMorph.MorphName = Morph->GetFName().ToString();

			for (auto& MorphLODModel : Morph->MorphLODModels)
			{
				FCloneMorphLODModel NewCloneMorphLOD;
				NewCloneMorphLOD.NumMeshVerts = MorphLODModel.NumBaseMeshVerts;
				NewCloneMorphLOD.SectionIndices = MorphLODModel.SectionIndices;
				NewCloneMorphLOD.Vertices = MorphLODModel.Vertices;

				NewCloneMorph.CloneMorphLODModels.Add(NewCloneMorphLOD);
			}

			AsyncTransferMorphsUtility->CloneMorphs.Add(NewCloneMorph);
		}

	}
	

	TFunction<void()> _TaskFunc = [AsyncTransferMorphsUtility]() {
		AsyncTransferMorphsUtility->AsyncTransferMorphs();
	};
	ENamedThreads::Type _Thread = ENamedThreads::AnyBackgroundHiPriTask;
	FFunctionGraphTask::CreateAndDispatchWhenReady(_TaskFunc, TStatId(), nullptr, _Thread);

	
	
}



void URuntimeCopyMorphToolsBPLibrary::SaveMorphs(USkeletalMesh* TargetMesh, TArray<FString> MorphNames, FString FileName, FDelegate Done)
{
	if (!IsValid(TargetMesh))return;

	UAsyncSaveMorphsUtility* AsyncSaveMorphsUtility = NewObject<UAsyncSaveMorphsUtility>();
	AsyncSaveMorphsUtility->AddToRoot();

	AsyncSaveMorphsUtility->FileName = FileName;
	AsyncSaveMorphsUtility->Delegate = Done;
	
	for (auto& Morph: TargetMesh->MorphTargets)
	{
		if (MorphNames.Contains(Morph->GetFName().ToString()))
		{
			FCloneMorph NewCloneMorph;
			NewCloneMorph.MorphName = Morph->GetFName().ToString();
			
			for (auto& MorphLODModel: Morph->MorphLODModels)
			{
				FCloneMorphLODModel NewCloneMorphLOD;
				NewCloneMorphLOD.NumMeshVerts = MorphLODModel.NumBaseMeshVerts;
				NewCloneMorphLOD.SectionIndices = MorphLODModel.SectionIndices;
				NewCloneMorphLOD.Vertices = MorphLODModel.Vertices;

				NewCloneMorph.CloneMorphLODModels.Add(NewCloneMorphLOD);
			}

			AsyncSaveMorphsUtility->CloneMorphs.Add(NewCloneMorph);
		}
		
	}

	TFunction<void()> _TaskFunc = [AsyncSaveMorphsUtility]() {
		AsyncSaveMorphsUtility->AsyncSaveMorphs();
	};
	ENamedThreads::Type _Thread = ENamedThreads::AnyBackgroundHiPriTask;
	FFunctionGraphTask::CreateAndDispatchWhenReady(_TaskFunc, TStatId(), nullptr, _Thread);

}



void URuntimeCopyMorphToolsBPLibrary::LoadMorphs(USkeletalMesh* TargetMesh, FString FileName, FDelegate Done, bool Overwrite)
{
	if (!IsValid(TargetMesh))return;

	UAsyncLoadMorphsUtility* AsyncLoadMorphsUtility = NewObject<UAsyncLoadMorphsUtility>();
	AsyncLoadMorphsUtility->AddToRoot();

	AsyncLoadMorphsUtility->TargetMesh = TargetMesh;
	AsyncLoadMorphsUtility->FileName = FileName;
	AsyncLoadMorphsUtility->Delegate = Done;
	AsyncLoadMorphsUtility->Overwrite = Overwrite;

	for (auto& Morph: TargetMesh->MorphTargets)
	{
		AsyncLoadMorphsUtility->CloneMorphNames.Add(Morph->GetFName().ToString());
	}
	
	TFunction<void()> _TaskFunc = [AsyncLoadMorphsUtility]() {
		AsyncLoadMorphsUtility->AsyncLoadMorphs();
	};
	ENamedThreads::Type _Thread = ENamedThreads::AnyBackgroundHiPriTask;
	FFunctionGraphTask::CreateAndDispatchWhenReady(_TaskFunc, TStatId(), nullptr, _Thread);
}



void URuntimeCopyMorphToolsBPLibrary::BakeMorphs(USkeletalMeshComponent* TargetMeshComponent, TArray<FString> MorphNames, FString NewMorphName, FDelegate Done)
{
	if (!IsValid(TargetMeshComponent))return;
	if (!IsValid(TargetMeshComponent->SkeletalMesh))return;

	UAsyncBakeMorphsUtility* AsyncBakeMorphsUtility = NewObject<UAsyncBakeMorphsUtility>();
	AsyncBakeMorphsUtility->AddToRoot();

	AsyncBakeMorphsUtility->TargetMesh = TargetMeshComponent->SkeletalMesh;
	AsyncBakeMorphsUtility->NewMorphName = NewMorphName;
	AsyncBakeMorphsUtility->Delegate = Done;

	for (auto& Morph : TargetMeshComponent->SkeletalMesh->MorphTargets)
	{
		if (MorphNames.Contains(Morph->GetFName().ToString()))
		{
			FCloneMorph NewCloneMorph;
			NewCloneMorph.MorphName = Morph->GetFName().ToString();
			NewCloneMorph.MorphWeight = TargetMeshComponent->MorphTargetWeights[*TargetMeshComponent->SkeletalMesh->MorphTargetIndexMap.Find(Morph->GetFName())];


			for (auto& MorphLODModel : Morph->MorphLODModels)
			{
				FCloneMorphLODModel NewCloneMorphLOD;
				NewCloneMorphLOD.NumMeshVerts = MorphLODModel.NumBaseMeshVerts;
				NewCloneMorphLOD.SectionIndices = MorphLODModel.SectionIndices;
				NewCloneMorphLOD.Vertices = MorphLODModel.Vertices;

				NewCloneMorph.CloneMorphLODModels.Add(NewCloneMorphLOD);
			}

			AsyncBakeMorphsUtility->CloneMorphs.Add(NewCloneMorph);
		}

	}

	for (auto& LODRenderData: TargetMeshComponent->SkeletalMesh->GetResourceForRendering()->LODRenderData)
	{
		FCloneLODRenderData CloneLODRenderData;

		CloneLODRenderData.RenderSectionsNum = LODRenderData.RenderSections.Num();
		CloneLODRenderData.Vertices.Reserve((uint32)LODRenderData.GetNumVertices());
		for (int32 LODVertexIndex = 0; LODVertexIndex < (int32)LODRenderData.GetNumVertices(); LODVertexIndex++)
		{
			FCloneLODRenderDataVertex CloneLODRenderDataVertex;
			CloneLODRenderDataVertex.TangentZ = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(LODVertexIndex);

			CloneLODRenderData.Vertices.Add(CloneLODRenderDataVertex);
		}

		AsyncBakeMorphsUtility->CloneLODRenderDatas.Add(CloneLODRenderData);
	}

	TFunction<void()> _TaskFunc = [AsyncBakeMorphsUtility]() {
		AsyncBakeMorphsUtility->AsyncBakeMorphs();
	};
	ENamedThreads::Type _Thread = ENamedThreads::AnyBackgroundHiPriTask;
	FFunctionGraphTask::CreateAndDispatchWhenReady(_TaskFunc, TStatId(), nullptr, _Thread);
}



void URuntimeCopyMorphToolsBPLibrary::CopyMorphs(USkeletalMesh* TargetMesh, USkeletalMesh* CopyFromMesh, TArray<FString> MorphNames, FString VertexWeightMapFileName, FDelegate Done, float Threshold, float SameVerticesThreshold, int32 SmoothCount, int32 AdditionalMultiplyCount, float UniformInflateMagnitude, bool Overwrite)
{
	if (!IsValid(TargetMesh) || !IsValid(CopyFromMesh))return;

	UAsyncCopyMorphsUtility* AsyncCopyMorphsUtility = NewObject<UAsyncCopyMorphsUtility>();
	AsyncCopyMorphsUtility->AddToRoot();

	AsyncCopyMorphsUtility->TargetMesh = TargetMesh;
	AsyncCopyMorphsUtility->SmoothCount = SmoothCount;
	AsyncCopyMorphsUtility->AdditionalMultiplyCount = AdditionalMultiplyCount;
	AsyncCopyMorphsUtility->UniformInflateMagnitude = UniformInflateMagnitude;
	AsyncCopyMorphsUtility->Threshold = Threshold;
	AsyncCopyMorphsUtility->SameVerticesThreshold = SameVerticesThreshold;
	AsyncCopyMorphsUtility->VertexWeightMapFileName = VertexWeightMapFileName;
	AsyncCopyMorphsUtility->Delegate = Done;
	AsyncCopyMorphsUtility->Overwrite = Overwrite;


	TSet<FName> TargetMeshMorphNamesSet;
	TSet<FName> CopyFromMeshMorphNamesSet;
	TSet<FName> MorphNamesSet;

	for (auto& Morph : TargetMesh->MorphTargets) { TargetMeshMorphNamesSet.Add(Morph->GetFName()); }
	for (auto& Morph : CopyFromMesh->MorphTargets) { CopyFromMeshMorphNamesSet.Add(Morph->GetFName()); }
	for (auto& MorphName : MorphNames) { MorphNamesSet.Add((FName)*MorphName); }

	TSet<FName> IntesectCopyFromAndMorph = CopyFromMeshMorphNamesSet.Intersect(MorphNamesSet);
	if (!(IntesectCopyFromAndMorph.Num() > 0))return;


	TSet<FName> MorphNamesToAdd;

	/*
	TArray<FString> MorphNamesToRemove;
	TArray<FCloneMorph> CopyFromCloneMorphs;
	TArray<FCloneLODRenderData> TargetCloneLODRenderDatas;
	TArray<FCloneLODIndices> TargetCloneLODsIndices;
	TArray<FCloneLODRenderData> CopyFromCloneLODRenderDatas;
	TArray<FCloneLODIndices> CopyFromCloneLODsIndices;
	TMap<FString, UMorphTarget*> NewMorphTargetsMap;
	*/

	if (Overwrite)
	{
		TSet<FName> IntersectAll = TargetMeshMorphNamesSet.Intersect(IntesectCopyFromAndMorph);
		//for (auto& MorphName : IntersectAll)UE_LOG(LogTemp, Warning, TEXT("Intersect%s"), *MorphName.ToString());

		if (IntersectAll.Num() > 0)
		{
			for (auto& MorphName : IntersectAll)
			{
				AsyncCopyMorphsUtility->MorphNamesToRemove.Add(MorphName.ToString());
			}


			MorphNamesToAdd = IntesectCopyFromAndMorph;

		}
		else
		{
			MorphNamesToAdd = IntesectCopyFromAndMorph;

		}
	}
	else
	{
		MorphNamesToAdd = IntesectCopyFromAndMorph.Difference(TargetMeshMorphNamesSet);

	}

	for (auto& Morph : CopyFromMesh->MorphTargets)
	{
		if (MorphNamesToAdd.Contains(Morph->GetFName()))
		{
			FCloneMorph NewCloneMorph;
			NewCloneMorph.MorphName = Morph->GetFName().ToString();

			for (auto& MorphLODModel : Morph->MorphLODModels)
			{
				FCloneMorphLODModel NewCloneMorphLOD;
				NewCloneMorphLOD.NumMeshVerts = MorphLODModel.NumBaseMeshVerts;
				NewCloneMorphLOD.SectionIndices = MorphLODModel.SectionIndices;
				NewCloneMorphLOD.Vertices = MorphLODModel.Vertices;

				NewCloneMorph.CloneMorphLODModels.Add(NewCloneMorphLOD);
			}

			AsyncCopyMorphsUtility->CopyFromCloneMorphs.Add(NewCloneMorph);
		}

	}

	//UE_LOG(LogTemp, Warning, TEXT("CloneMorphLODVerticesNUM_%d"), AsyncCopyMorphsUtility->CopyFromCloneMorphs[0].CloneMorphLODModels[1].Vertices.Num());

	for (auto& LODRenderData : TargetMesh->GetResourceForRendering()->LODRenderData)
	{
		FCloneLODRenderData CloneLODRenderData;

		CloneLODRenderData.RenderSectionsNum = LODRenderData.RenderSections.Num();

		CloneLODRenderData.Vertices.Reserve((uint32)LODRenderData.GetNumVertices());
		for (int32 LODVertexIndex = 0; LODVertexIndex < (int32)LODRenderData.GetNumVertices(); LODVertexIndex++)
		{
			FCloneLODRenderDataVertex CloneLODRenderDataVertex;
			CloneLODRenderDataVertex.TangentZ = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(LODVertexIndex);
			CloneLODRenderDataVertex.VertexPosition = LODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(LODVertexIndex);

			CloneLODRenderData.Vertices.Add(CloneLODRenderDataVertex);

		}

		AsyncCopyMorphsUtility->TargetCloneLODRenderDatas.Add(CloneLODRenderData);

		FCloneLODIndices CloneLODIndices;

		LODRenderData.MultiSizeIndexContainer.GetIndexBuffer(CloneLODIndices.LODIndices);

		AsyncCopyMorphsUtility->TargetCloneLODsIndices.Add(CloneLODIndices);
	}

	for (auto& LODRenderData : CopyFromMesh->GetResourceForRendering()->LODRenderData)
	{
		FCloneLODRenderData CloneLODRenderData;

		CloneLODRenderData.RenderSectionsNum = LODRenderData.RenderSections.Num();

		CloneLODRenderData.Vertices.Reserve((uint32)LODRenderData.GetNumVertices());
		for (int32 LODVertexIndex = 0; LODVertexIndex < (int32)LODRenderData.GetNumVertices(); LODVertexIndex++)
		{
			FCloneLODRenderDataVertex CloneLODRenderDataVertex;
			CloneLODRenderDataVertex.TangentZ = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(LODVertexIndex);
			CloneLODRenderDataVertex.VertexPosition = LODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(LODVertexIndex);

			CloneLODRenderData.Vertices.Add(CloneLODRenderDataVertex);

		}

		AsyncCopyMorphsUtility->CopyFromCloneLODRenderDatas.Add(CloneLODRenderData);

		FCloneLODIndices CloneLODIndices;

		LODRenderData.MultiSizeIndexContainer.GetIndexBuffer(CloneLODIndices.LODIndices);

		AsyncCopyMorphsUtility->CopyFromCloneLODsIndices.Add(CloneLODIndices);
	}
	
	TFunction<void()> _TaskFunc = [AsyncCopyMorphsUtility]() {
		AsyncCopyMorphsUtility->AsyncCopyMorphs();
	};
	ENamedThreads::Type _Thread = ENamedThreads::AnyBackgroundHiPriTask;
	
	//if(!TaskQueue)UE_LOG(LogTemp, Warning, TEXT("TaskQueIsFalse"));
	//TaskQueue.Num();
	
	
	URuntimeCopyMorphToolsSingleton* CopyMorphsSingleton = GetRuntimeCopyMorphToolsSingleton();
	if (!CopyMorphsSingleton->AsyncCopyMorphsTasks.IsValidIndex(CopyMorphsSingleton->AsyncCopyMorphsTasks.Num() - 1)/*!CopyMorphsSingleton->AsyncCopyMorphsTasks.Num() == 0*/)
	{
		CopyMorphsSingleton->AsyncCopyMorphsTasks.Empty();
		FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady(_TaskFunc, TStatId(), nullptr, _Thread);
		CopyMorphsSingleton->AsyncCopyMorphsTasks.Add(Task);
		UE_LOG(LogTemp, Warning, TEXT("CopyMorphsTaskLast_InValid"));
	}
	else
	{
		//if(CopyMorphsSingleton->AsyncCopyMorphsTasks.IsValidIndex(CopyMorphsSingleton->AsyncCopyMorphsTasks.Num() - 1))
		if (!CopyMorphsSingleton->AsyncCopyMorphsTasks.Last()->IsComplete())
		{
			
			FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady(_TaskFunc, TStatId(), CopyMorphsSingleton->AsyncCopyMorphsTasks.Last(), _Thread);
			CopyMorphsSingleton->AsyncCopyMorphsTasks.Add(Task);
			UE_LOG(LogTemp, Warning, TEXT("CopyMorphsTaskLast_IsNotComplete"));
		}
		else
		{
			CopyMorphsSingleton->AsyncCopyMorphsTasks.Empty();
			FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady(_TaskFunc, TStatId(), nullptr, _Thread);
			CopyMorphsSingleton->AsyncCopyMorphsTasks.Add(Task);
			UE_LOG(LogTemp, Warning, TEXT("CopyMorphsTaskLast_IsComplete"));
		}
	}
	

	//TaskQueue = Task;

	//FTaskGraphInterface::Get().WaitUntilTaskCompletes(Task, _Thread);
	/*
	auto AsyncTask = new FAutoDeleteAsyncTask<FAsyncExecTask>(AsyncCopyMorphs, TargetCloneLODRenderDatas, TargetCloneLODsIndices,
		CopyFromCloneMorphs, CopyFromCloneLODRenderDatas, CopyFromCloneLODsIndices,
		TargetMesh, SmoothCount, AdditionalMultiplyCount, Threshold, SameVerticesThreshold,
		MorphNamesToRemove, NewMorphTargetsMap, Done, Overwrite);
	AsyncTask->StartBackgroundTask();
	*/
}


/*
void URuntimeCopyMorphToolsBPLibrary::AsyncCopyMorphs(TArray<FCloneLODRenderData> TargetCloneLODRenderDatas, TArray<FCloneLODIndices> TargetCloneLODsIndices,
	TArray<FCloneMorph> CopyFromCloneMorphs, TArray<FCloneLODRenderData> CopyFromCloneLODRenderDatas, TArray<FCloneLODIndices> CopyFromCloneLODsIndices,
	USkeletalMesh* TargetMesh, int32 SmoothCount, int32 AdditionalMultiplyCount, float Threshold, float SameVerticesThreshold,
	TArray<FString> MorphNamesToRemove, TMap<FString, UMorphTarget*> NewMorphTargetsMap, FDelegate Delegate, bool Overwrite)
{
	UE_LOG(LogTemp, Warning, TEXT("StartCreateMorphs"));
	float SmoothStrength = 0.9;
	int32 ExtraDeltaCount = AdditionalMultiplyCount;

	//TMap<FString, UMorphTarget*> NewMorphTargetsMap;

	uint32 LODNumber = (uint32)FMath::Min<int32>(TargetCloneLODRenderDatas.Num(), CopyFromCloneLODRenderDatas.Num());


	//UE_LOG(LogTemp, Warning, TEXT("LODNumber%d"), LODNumber);

	TArray<FString> TargetMorphNames;
	for (auto& TargetCloneMorph : CopyFromCloneMorphs)
	{
		TargetMorphNames.Add(TargetCloneMorph.MorphName);
	}

	for (uint32 LODIndex = 0; LODIndex < LODNumber; LODIndex++)
	{
		//UE_LOG(LogTemp, Warning, TEXT("LOD%dstart"), LODIndex);

		TArray<FVertexWeight> VertexWeights;


		TArray<FSameVerticesSet> TargetMeshSameVerticesSetArr;
		TMap<uint32, uint32> TargetToBaseMeshSameVertexMap;





		for (auto& MorphName : TargetMorphNames)
		{
			UMorphTarget* NewMorph;
			if (LODIndex == 0)
			{
				NewMorph = NewObject<UMorphTarget>();
				NewMorph->BaseSkelMesh = TargetMesh;
			}
			else {
				if (NewMorphTargetsMap.Contains(MorphName))
				{
					UE_LOG(LogTemp, Warning, TEXT("FoundSameNameMorph"));
					NewMorph = *NewMorphTargetsMap.Find(MorphName);
				}
				else {
					continue;
				}

			}


			if (!VertexWeights.IsValidIndex(0))
			{

				for (uint32 TargetIndex = 0; TargetIndex < (uint32)TargetCloneLODRenderDatas[LODIndex].Vertices.Num(); TargetIndex++)
				{
					//UE_LOG(LogTemp, Warning, TEXT("targetclonelodrenderdataverticesnum %d"), TargetCloneLODRenderDatas[LODIndex].Vertices.Num());
					FVector TargetVertexPosition = TargetCloneLODRenderDatas[LODIndex].Vertices[TargetIndex].VertexPosition;//TargetLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(TargetIndex);
					TArray<uint32> CloseTargetVertexIndices;
					for (uint32 SecondTargetIndex = TargetIndex; SecondTargetIndex < (uint32)TargetCloneLODRenderDatas[LODIndex].Vertices.Num(); SecondTargetIndex++)
					{
						FVector SecondTargetVertexPosition = TargetCloneLODRenderDatas[LODIndex].Vertices[SecondTargetIndex].VertexPosition;//TargetLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(SecondTargetIndex);
						if ((TargetVertexPosition.Y - 0.001 < SecondTargetVertexPosition.Y && SecondTargetVertexPosition.Y < TargetVertexPosition.Y + 0.001)
							&& (TargetVertexPosition.Z - 0.001 < SecondTargetVertexPosition.Z && SecondTargetVertexPosition.Z < TargetVertexPosition.Z + 0.001)
							&& (TargetVertexPosition.X - 0.001 < SecondTargetVertexPosition.X && SecondTargetVertexPosition.X < TargetVertexPosition.X + 0.001))
						{
							bool FoundVerticesSet = false;
							for (auto& VerticesSet : TargetMeshSameVerticesSetArr)
							{
								if (VerticesSet.SameVertices.Contains(TargetIndex) || VerticesSet.SameVertices.Contains(SecondTargetIndex))
								{
									VerticesSet.SameVertices.Add(TargetIndex);
									VerticesSet.SameVertices.Add(SecondTargetIndex);
									FoundVerticesSet = true;
									break;
								}
							}
							if (!FoundVerticesSet)
							{
								TargetMeshSameVerticesSetArr.Add(FSameVerticesSet({ {TargetIndex, SecondTargetIndex} }));
							}
						}
					}

				}








				//UE_LOG(LogTemp, Warning, TEXT("NoValidVertexWeights"));
				//make vertex weight map
				for (uint32 TargetIndex = 0; TargetIndex < (uint32)TargetCloneLODRenderDatas[LODIndex].Vertices.Num(); TargetIndex++)
				{
					bool ContinueBool = 0;

					FVector TargetVertexPosition = TargetCloneLODRenderDatas[LODIndex].Vertices[TargetIndex].VertexPosition;//TargetLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(TargetIndex);
					TArray<uint32> CloseBaseVertexIndices;
					//add close vertices to array
					for (uint32 BaseVertexIndex = 0; BaseVertexIndex < (uint32)CopyFromCloneLODRenderDatas[LODIndex].Vertices.Num(); BaseVertexIndex++)
					{
						FVector BaseVertexPosition = CopyFromCloneLODRenderDatas[LODIndex].Vertices[BaseVertexIndex].VertexPosition;//BaseLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(BaseVertexIndex);

						if ((TargetVertexPosition.Y - Threshold < BaseVertexPosition.Y && BaseVertexPosition.Y < TargetVertexPosition.Y + Threshold)
							&& (TargetVertexPosition.Z - Threshold < BaseVertexPosition.Z && BaseVertexPosition.Z < TargetVertexPosition.Z + Threshold)
							&& (TargetVertexPosition.X - Threshold < BaseVertexPosition.X && BaseVertexPosition.X < TargetVertexPosition.X + Threshold))
						{
							CloseBaseVertexIndices.Add(BaseVertexIndex);
						}
					}


					//add false vertexweight and continue if closevertexindices have no element
					if (CloseBaseVertexIndices.Num() == 0)
					{
						VertexWeights.Add(FVertexWeight{ false,0,0,0,0,0,0 });

						continue;
					}





					float VerticesGap = Threshold * 3;
					uint32 ClosestVertexIndex;
					//find same or closest vertex
					for (uint32 BaseVertexIndex : CloseBaseVertexIndices)
					{
						FVector BaseVertexPosition = CopyFromCloneLODRenderDatas[LODIndex].Vertices[BaseVertexIndex].VertexPosition;//BaseLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(BaseVertexIndex);
						float NewVerticesGap = (TargetVertexPosition - BaseVertexPosition).Size();

						//same vertex
						if (NewVerticesGap < SameVerticesThreshold)
						{
							VertexWeights.Add(FVertexWeight{ true,(int32)TargetIndex,1,0,0,0,0 });
							TargetToBaseMeshSameVertexMap.Add(TargetIndex, BaseVertexIndex);

							ContinueBool = 1;
							break;
						}
						else if (NewVerticesGap < VerticesGap)
						{
							VerticesGap = NewVerticesGap;
							ClosestVertexIndex = BaseVertexIndex;
						}

					}



					//continue if there is same vertex
					if (ContinueBool)
					{
						continue;
					}





					//find closest face



					//find closest side




					//float vertextestweight = 1 - (TargetVertexPosition - BaseLODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(ClosestVertexIndex)).Size() / Threshold;
					//only closest vertex exist
					VertexWeights.Add(FVertexWeight{ true,(int32)ClosestVertexIndex,1.0,0,0,0,0 });
					//UE_LOG(LogTemp, Warning, TEXT("VertexIndex%d"), ClosestVertexIndex);

					continue;

				}
			}


			//make new morph map


			FMorphTargetLODModel NewLODMorph;
			NewLODMorph.bGeneratedByEngine = false;
			NewLODMorph.NumBaseMeshVerts = TargetCloneLODRenderDatas[LODIndex].Vertices.Num();//TargetLODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.GetNumVertices();
			for (int32 i = 0; i < TargetCloneLODRenderDatas[LODIndex].RenderSectionsNum; i++)
			{
				NewLODMorph.SectionIndices.Add(i);
			}

			//CopyFromCloneMorphs[0].CloneMorphLODModels[LODIndex].

			uint32 CopyFromMeshMorphIndex;
			bool CopyFromCloneMorphsHaveMorphName = false;
			for (uint32 CopyFromCloneMorphIndex = 0; CopyFromCloneMorphIndex < (uint32)CopyFromCloneMorphs.Num(); CopyFromCloneMorphIndex++)
			{
				if (CopyFromCloneMorphs[CopyFromCloneMorphIndex].MorphName == MorphName)
				{
					CopyFromCloneMorphsHaveMorphName = true;
					CopyFromMeshMorphIndex = CopyFromCloneMorphIndex;
					break;
				}
			}
			if (!CopyFromCloneMorphsHaveMorphName)continue;
			//if (!BaseMesh->FindMorphTarget((FName)*MorphName)->MorphLODModels.IsValidIndex(LODIndex)) { continue; }

			//FMorphTargetLODModel& BaseMeshMorph = BaseMesh->FindMorphTarget((FName)*MorphName)->MorphLODModels[LODIndex];

			for (uint32 index = 0; index < (uint32)NewLODMorph.NumBaseMeshVerts; index++)
			{
				//UE_LOG(LogTemp, Warning, TEXT("vertexindexandweight %d,%f"),VertexWeights[index].FirstVertexIndex,VertexWeights[index].FirstVertexWeight);
				if (VertexWeights[index].HaveAnyWeight == false) { continue; }

				FMorphTargetDelta NewDelta;
				NewDelta.PositionDelta = FVector{ 0,0,0 };
				NewDelta.SourceIdx = index;
				//NewDelta.PositionDelta = FVector(100,0,0);

				bool BasemeshMorphHaveValidVertex = false;
				for (uint32 BasemeshMorphVertexIndex = 0; BasemeshMorphVertexIndex
					< (uint32)CopyFromCloneMorphs[CopyFromMeshMorphIndex].CloneMorphLODModels[LODIndex].Vertices.Num()
					; BasemeshMorphVertexIndex++)
				{


					if (CopyFromCloneMorphs[CopyFromMeshMorphIndex].CloneMorphLODModels[LODIndex].Vertices[BasemeshMorphVertexIndex].SourceIdx
						 == VertexWeights[index].FirstVertexIndex)
					{
						NewDelta.PositionDelta += CopyFromCloneMorphs[CopyFromMeshMorphIndex].CloneMorphLODModels[LODIndex].Vertices[BasemeshMorphVertexIndex].PositionDelta
							* VertexWeights[index].FirstVertexWeight;
						//NewDelta.TangentZDelta +=  BaseLODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(VertexWeights[index].FirstVertexIndex);
						//if(VertexWeights[index].FirstVertexWeight < 1)UE_LOG(LogTemp, Warning, TEXT("WeightOver1"));
						BasemeshMorphHaveValidVertex = true;
					}
					
				}

				if (!BasemeshMorphHaveValidVertex) { continue; }

				//UE_LOG(LogTemp, Warning, TEXT("newdelta %f,%f,%f"), NewDelta.PositionDelta.X, NewDelta.PositionDelta.Y, NewDelta.PositionDelta.Z);

				NewLODMorph.Vertices.Add(NewDelta);
			}



			//smooth deltas

			for (int smoothnumber = 0; smoothnumber < SmoothCount; smoothnumber++)
			{

				TArray<FVector> NewDeltasArr;
				for (uint32 i = 0; i < (uint32)TargetCloneLODRenderDatas[LODIndex].Vertices.Num(); i++)
				{
					NewDeltasArr.Add(FVector(0, 0, 0));
				}

				for (FMorphTargetDelta& delta : NewLODMorph.Vertices)
				{
					NewDeltasArr[delta.SourceIdx] = delta.PositionDelta;
				}

				TArray<TArray<FVector>> TempSmoothedDeltasArr;
				for (uint32 i = 0; i < (uint32)TargetCloneLODRenderDatas[LODIndex].Vertices.Num(); i++)
				{
					TempSmoothedDeltasArr.Add(TArray<FVector>());
				}

				for (auto& delta : NewLODMorph.Vertices)
				{
					//copydeltas.Add(delta);
					for (uint32 TargetLODindex = 0; TargetLODindex < (uint32)TargetCloneLODsIndices[LODIndex].LODIndices.Num(); TargetLODindex += 3)
					{
						if (delta.SourceIdx == TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex]
							|| delta.SourceIdx == TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 1]
							|| delta.SourceIdx == TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 2])
						{
							FVector FirstVertexDelta = ((NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 1]]
								+ NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 2]]) / 2
								- NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex]]) * SmoothStrength + NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex]];

							FVector SecondVertexDelta = ((NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 2]]
								+ NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex]]) / 2
								- NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 1]]) * SmoothStrength + NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 1]];

							FVector ThirdVertexDelta = ((NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex]]
								+ NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 1]]) / 2
								- NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 2]]) * SmoothStrength + NewDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 2]];


							TempSmoothedDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex]].Add(FirstVertexDelta);

							TempSmoothedDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 1]].Add(SecondVertexDelta);

							TempSmoothedDeltasArr[TargetCloneLODsIndices[LODIndex].LODIndices[TargetLODindex + 2]].Add(ThirdVertexDelta);




						}
					}
				}





				NewLODMorph.Vertices.Empty();
				for (uint32 i = 0; i < (uint32)TempSmoothedDeltasArr.Num(); i++)
				{
					if (TempSmoothedDeltasArr[i].Num() > 0)
					{
						FMorphTargetDelta NewDelta;
						NewDelta.SourceIdx = i;
						NewDelta.PositionDelta = FVector(0, 0, 0);

						for (FVector tempvector : TempSmoothedDeltasArr[i])
						{
							NewDelta.PositionDelta += tempvector;
						}

						NewDelta.PositionDelta /= TempSmoothedDeltasArr[i].Num();

						NewLODMorph.Vertices.Add(NewDelta);
					}
				}

			}



			//fix targetmesh same vertices deltas
			for (FSameVerticesSet& SameVertices : TargetMeshSameVerticesSetArr)
			{
				FVector SumDeltas = { 0,0,0 };
				//UE_LOG(LogTemp, Warning, TEXT("sameverticesnum%d"), SameVertices.SameVertices.Num());

				for (uint32 TargetLODMorphIndex = 0; TargetLODMorphIndex < (uint32)NewLODMorph.Vertices.Num(); TargetLODMorphIndex++)
				{
					if (SameVertices.SameVertices.Contains(NewLODMorph.Vertices[TargetLODMorphIndex].SourceIdx))
					{
						SumDeltas += NewLODMorph.Vertices[TargetLODMorphIndex].PositionDelta;
					}
				}

				FVector AverageDelta = SumDeltas / SameVertices.SameVertices.Num();

				for (uint32 TargetLODMorphIndex = 0; TargetLODMorphIndex < (uint32)NewLODMorph.Vertices.Num(); TargetLODMorphIndex++)
				{
					if (SameVertices.SameVertices.Contains(NewLODMorph.Vertices[TargetLODMorphIndex].SourceIdx))
					{
						NewLODMorph.Vertices[TargetLODMorphIndex].PositionDelta = AverageDelta;
					}
				}


			}






			//fix same vertex delta
			for (FMorphTargetDelta& newdelta : NewLODMorph.Vertices)
			{
				//UE_LOG(LogTemp, Warning, TEXT("newdelta %f,%f,%f"), newdelta.PositionDelta.X, newdelta.PositionDelta.Y, newdelta.PositionDelta.Z);
				if (TargetToBaseMeshSameVertexMap.Contains(newdelta.SourceIdx))
				{
					newdelta.PositionDelta = FVector(0, 0, 0);


					for (FMorphTargetDelta& basedelta : CopyFromCloneMorphs[CopyFromMeshMorphIndex].CloneMorphLODModels[LODIndex].Vertices)
					{
						if (basedelta.SourceIdx == TargetToBaseMeshSameVertexMap[newdelta.SourceIdx])
						{
							newdelta.PositionDelta = basedelta.PositionDelta;

							break;
						}
					}

				}
				else
				{
					for (int32 i = 0; i < ExtraDeltaCount; i++)
					{
						newdelta.PositionDelta *= 1.1;
					}
				}
			}


			//fix tanZnormal
			for (FMorphTargetDelta& newdelta : NewLODMorph.Vertices)
			{
				//UE_LOG(LogTemp, Warning, TEXT("newdelta %f,%f,%f"), newdelta.PositionDelta.X, newdelta.PositionDelta.Y, newdelta.PositionDelta.Z);
				newdelta.TangentZDelta = TargetCloneLODRenderDatas[LODIndex].Vertices[newdelta.SourceIdx].TangentZ;//TargetLODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(newdelta.SourceIdx);
			}


			//UE_LOG(LogTemp, Warning, TEXT("LOD%dfinish"), LODIndex);

			//FString NewString = MorphName.ToString();
			NewMorph->MorphLODModels.Add(NewLODMorph);
			if (LODIndex == 0) { NewMorphTargetsMap.Add(MorphName, NewMorph); }

		}

	}


	UInitMorphsAndResourcesUtility* InitMorphsAndResourcesUtility = NewObject<UInitMorphsAndResourcesUtility>();
	InitMorphsAndResourcesUtility->AddToRoot();

	InitMorphsAndResourcesUtility->TargetMesh = TargetMesh;
	InitMorphsAndResourcesUtility->MorphNamesToRemove = MorphNamesToRemove;
	InitMorphsAndResourcesUtility->MorphNameAndMorphTargetMap = NewMorphTargetsMap;
	InitMorphsAndResourcesUtility->Delegate = Delegate;


	TFunction<void()> _TaskFunc = [InitMorphsAndResourcesUtility]() {
		InitMorphsAndResourcesUtility->InitMorphsAndResources();
	};
	ENamedThreads::Type _Thread = ENamedThreads::GameThread;
	FFunctionGraphTask::CreateAndDispatchWhenReady(_TaskFunc, TStatId(), nullptr, _Thread);


	//TargetMesh->InitResources();
	UE_LOG(LogTemp, Warning, TEXT("FinishCreateMorphs"));
}
*/



/*
void URuntimeCopyMorphToolsBPLibrary::UniformInflateMorphs(USkeletalMesh* TargetMesh, TArray<FString> MorphNames, float Magnitude, FDelegate Done)
{
	if (!IsValid(TargetMesh))return;

	UAsyncUniformInflateMorphsUtility* AsyncUniformInflateMorphsUtility = NewObject<UAsyncUniformInflateMorphsUtility>();
	AsyncUniformInflateMorphsUtility->AddToRoot();

}
*/







URuntimeCopyMorphToolsSingleton* URuntimeCopyMorphToolsBPLibrary::RuntimeCopyMorphToolsSingleton;



URuntimeCopyMorphToolsSingleton* URuntimeCopyMorphToolsBPLibrary::GetRuntimeCopyMorphToolsSingleton()
{
	if (!IsValid(RuntimeCopyMorphToolsSingleton))
	{
		URuntimeCopyMorphToolsSingleton* NewSingleton = NewObject<URuntimeCopyMorphToolsSingleton>();
		NewSingleton->AddToRoot();
		RuntimeCopyMorphToolsSingleton = NewSingleton;

		return NewSingleton;
	}
	
	return RuntimeCopyMorphToolsSingleton;
}



UMorphTarget* URuntimeCopyMorphToolsBPLibrary::GetMorphByName(TArray<UMorphTarget*>& Morphs, FName Name)
{
	for (auto& Morph: Morphs)
	{
		if (Morph->GetFName() == Name)
		{
			return Morph;
		}
	}
	return nullptr;
}