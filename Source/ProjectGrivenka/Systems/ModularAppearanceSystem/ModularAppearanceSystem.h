// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularAppearanceSystemAvailable.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "ProjectGrivenka/Equipments/EquipmentDefinitions.h"
#include "ModularAppearanceSystem.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSkelMeshMergeSectionMapping_BP
{
    GENERATED_BODY()
        /** Indices to final section entries of the merged skeletal mesh */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Merge Params")
        TArray < int32 > SectionIDs;
};
/**
* Used to wrap a set of UV Transforms for one mesh.
*/
USTRUCT(BlueprintType)
struct FSkelMeshMergeUVTransform
{
    GENERATED_BODY()
        /** A list of how UVs should be transformed on a given mesh, where index represents a specific UV channel. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Merge Params")
        TArray < FTransform > UVTransforms;
};
/**
* Blueprint equivalent of FSkelMeshMergeUVTransforms
* Info to map all the sections about how to transform their UVs
*/
USTRUCT(BlueprintType)
struct FSkelMeshMergeUVTransformMapping
{
    GENERATED_BODY()
        /** For each UV channel on each mesh, how the UVS should be transformed. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Merge Params")
        TArray < FSkelMeshMergeUVTransform > UVTransformsPerMesh;
};
/**
* Struct containing all parameters used to perform a Skeletal Mesh merge.
*/
USTRUCT(BlueprintType)
struct FSkeletalMeshMergeParams
{
    GENERATED_BODY()
        FSkeletalMeshMergeParams()
    {
        StripTopLODS = 0;
        bNeedsCpuAccess = false;
        bSkeletonBefore = false;
        Skeleton = nullptr;
    }
    // An optional array to map sections from the source meshes to merged section entries
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray < FSkelMeshMergeSectionMapping_BP > MeshSectionMappings;
    // An optional array to transform the UVs in each mesh
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray < FSkelMeshMergeUVTransformMapping > UVTransformsPerMesh;
    // The list of skeletal meshes to merge.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray < USkeletalMesh* > MeshesToMerge;
    // The number of high LODs to remove from input meshes
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 StripTopLODS;
    // Whether or not the resulting mesh needs to be accessed by the CPU for any reason (e.g. for spawning particle effects).
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        uint32 bNeedsCpuAccess : 1;
    // Update skeleton before merge. Otherwise, update after.
    // Skeleton must also be provided.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        uint32 bSkeletonBefore : 1;
    // Skeleton that will be used for the merged mesh.
    // Leave empty if the generated skeleton is OK.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        class USkeleton* Skeleton;
};


UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UModularAppearanceSystem : public UBaseContextableComponent
{
	GENERATED_BODY()
	

public:
    UFUNCTION(BlueprintCallable, Category = "Mesh Merge", meta = (UnsafeDuringActorConstruction = "true"))
    static class USkeletalMesh* MergeMeshes(const FSkeletalMeshMergeParams& Params);
    UFUNCTION(BlueprintCallable)
    void SaveAppearance(FPersistedCharacterAppearance InAppearance, FPersistedEquipments InEquipments, int InGender);
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void LoadAppearance(FPersistedCharacterAppearance InAppearance, FPersistedEquipments InEquipments, int InGender);
};