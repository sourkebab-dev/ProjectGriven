// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularAppearanceSystem.h"
#include "MorphToolsFunctions.h"
#include "SkeletalMeshMerge.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"


static void ToMergeParams(const TArray<FSkelMeshMergeSectionMapping_BP>& InSectionMappings, TArray<FSkelMeshMergeSectionMapping>& OutSectionMappings)
{
    if (InSectionMappings.Num() > 0)
    {
        OutSectionMappings.AddUninitialized(InSectionMappings.Num());
        for (int32 i = 0; i < InSectionMappings.Num(); ++i)
        {
            OutSectionMappings[i].SectionIDs = InSectionMappings[i].SectionIDs;
        }
    }
};

static void ToMergeParams(const TArray<FSkelMeshMergeUVTransformMapping>& InUVTransformsPerMesh, TArray<FSkelMeshMergeUVTransforms>& OutUVTransformsPerMesh)
{
    if (InUVTransformsPerMesh.Num() > 0)
    {
        OutUVTransformsPerMesh.Empty();
        OutUVTransformsPerMesh.AddUninitialized(InUVTransformsPerMesh.Num());
        for (int32 i = 0; i < InUVTransformsPerMesh.Num(); ++i)
        {
            TArray<TArray<FTransform>>& OutUVTransforms = OutUVTransformsPerMesh[i].UVTransformsPerMesh;
            const TArray<FSkelMeshMergeUVTransform>& InUVTransforms = InUVTransformsPerMesh[i].UVTransformsPerMesh;
            if (InUVTransforms.Num() > 0)
            {
                OutUVTransforms.Empty();
                OutUVTransforms.AddUninitialized(InUVTransforms.Num());
                for (int32 j = 0; j < InUVTransforms.Num(); j++)
                {
                    OutUVTransforms[i] = InUVTransforms[i].UVTransforms;
                }
            }
        }
    }
};

USkeletalMesh* UModularAppearanceSystem::MergeMeshes(const FSkeletalMeshMergeParams& Params)
{
    TArray<USkeletalMesh*> MeshesToMergeCopy = Params.MeshesToMerge;
    MeshesToMergeCopy.RemoveAll([](USkeletalMesh* InMesh)
        {
            return InMesh == nullptr;
        });
    if (MeshesToMergeCopy.Num() <= 1)
    {
        UE_LOG(LogTemp, Warning, TEXT("Must provide multiple valid Skeletal Meshes in order to perform a merge."));
        return nullptr;
    }
    EMeshBufferAccess BufferAccess = Params.bNeedsCpuAccess ?
        EMeshBufferAccess::ForceCPUAndGPU :
        EMeshBufferAccess::Default;
    TArray<FSkelMeshMergeSectionMapping> SectionMappings;
    TArray<FSkelMeshMergeUVTransforms> UvTransforms;
    ToMergeParams(Params.MeshSectionMappings, SectionMappings);
    ToMergeParams(Params.UVTransformsPerMesh, UvTransforms);
    bool bRunDuplicateCheck = false;
    USkeletalMesh* BaseMesh = NewObject<USkeletalMesh>();
    if (Params.Skeleton && Params.bSkeletonBefore)
    {
        BaseMesh->Skeleton = Params.Skeleton;
        bRunDuplicateCheck = true;
        for (USkeletalMeshSocket* Socket : BaseMesh->GetMeshOnlySocketList())
        {
            if (Socket)
            {
                UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocket: %s"), *(Socket->SocketName.ToString()));
            }
        }
        for (USkeletalMeshSocket* Socket : BaseMesh->Skeleton->Sockets)
        {
            if (Socket)
            {
                UE_LOG(LogTemp, Warning, TEXT("SkelSocket: %s"), *(Socket->SocketName.ToString()));
            }
        }
    }
    FSkeletalMeshMerge Merger(BaseMesh, MeshesToMergeCopy, SectionMappings, Params.StripTopLODS, BufferAccess, UvTransforms.GetData());
    if (!Merger.DoMerge())
    {
        UE_LOG(LogTemp, Warning, TEXT("Merge failed!"));
        return nullptr;
    }
    if (Params.Skeleton && !Params.bSkeletonBefore)
    {
        BaseMesh->Skeleton = Params.Skeleton;
    }
    if (bRunDuplicateCheck)
    {
        TArray<FName> SkelMeshSockets;
        TArray<FName> SkelSockets;
        for (USkeletalMeshSocket* Socket : BaseMesh->GetMeshOnlySocketList())
        {
            if (Socket)
            {
                SkelMeshSockets.Add(Socket->GetFName());
                UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocket: %s"), *(Socket->SocketName.ToString()));
            }
        }
        for (USkeletalMeshSocket* Socket : BaseMesh->Skeleton->Sockets)
        {
            if (Socket)
            {
                SkelSockets.Add(Socket->GetFName());
                UE_LOG(LogTemp, Warning, TEXT("SkelSocket: %s"), *(Socket->SocketName.ToString()));
            }
        }
        TSet<FName> UniqueSkelMeshSockets;
        TSet<FName> UniqueSkelSockets;
        UniqueSkelMeshSockets.Append(SkelMeshSockets);
        UniqueSkelSockets.Append(SkelSockets);
        int32 Total = SkelSockets.Num() + SkelMeshSockets.Num();
        int32 UniqueTotal = UniqueSkelMeshSockets.Num() + UniqueSkelSockets.Num();
        UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocketCount: %d | SkelSocketCount: %d | Combined: %d"), SkelMeshSockets.Num(), SkelSockets.Num(), Total);
        UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocketCount: %d | SkelSocketCount: %d | Combined: %d"), UniqueSkelMeshSockets.Num(), UniqueSkelSockets.Num(), UniqueTotal);
        UE_LOG(LogTemp, Warning, TEXT("Found Duplicates: %s"), *((Total != UniqueTotal) ? FString("True") : FString("False")));
    }
  
    BaseMesh->RebuildSocketMap();
    return BaseMesh;
}

void UModularAppearanceSystem::Init_Implementation()
{
    Super::Init_Implementation();

    if (!this->GetOwner()->Implements<UModularAppearanceSystemAvailable>()) {
        GLog->Log(this->GetOwner()->GetFullName());
        UE_LOG(LogTemp, Error, TEXT("Modular Appearance Initiation Failure"), *GetNameSafe(this));
        return;
    }

    IModularAppearanceSystemAvailable::Execute_GetModularParts(this->CompContext->CharacterActor, this->ModularParts);
    this->MaterialIns = this->CompContext->SkeletalMeshComp->CreateDynamicMaterialInstance(0, this->MasterMaterial);
}

void UModularAppearanceSystem::SaveAppearance(FPersistedCharacterAppearance InAppearance, FPersistedEquipments InEquipments, int InGender)
{
}

void UModularAppearanceSystem::LoadAppearance(FPersistedCharacterAppearance InAppearance, FPersistedEquipments InEquipments, int InGender)
{
    this->LoadedAppearance = InAppearance;
    this->LoadedEquipments = InEquipments;
    this->Gender = InGender;

    this->InitiateAppearance();
    this->ApplyAndBakeMorphTargets();
}

void UModularAppearanceSystem::InitiateAppearance()
{
    //sponge: need to do hiddenby/replacedby
    UGrivenkaDataSingleton* GrivenkaSingleton = UGrivenkaSingletonLibrary::GetGrivenkaData();
    FWearableInfo HeadWearable = GrivenkaSingleton->GetWearableInfo(this->LoadedEquipments.HeadWearable.VariantId);
    FWearableInfo OuterTorsoWearable = GrivenkaSingleton->GetWearableInfo(this->LoadedEquipments.OuterTorsoWearable.VariantId);
    FWearableInfo TorsoWearable = GrivenkaSingleton->GetWearableInfo(this->LoadedEquipments.TorsoWearable.VariantId);
    FWearableInfo LegsWearable = GrivenkaSingleton->GetWearableInfo(this->LoadedEquipments.LegsWearable.VariantId);
    FWearableInfo HandsWearable = GrivenkaSingleton->GetWearableInfo(this->LoadedEquipments.HandsWearable.VariantId);
    FWearableInfo FootWearable = GrivenkaSingleton->GetWearableInfo(this->LoadedEquipments.FootWearable.VariantId);
    FWearableInfo Acc1Wearable = GrivenkaSingleton->GetWearableInfo(this->LoadedEquipments.Accessories1Wearable.VariantId);
    FWearableInfo Acc2Wearable = GrivenkaSingleton->GetWearableInfo(this->LoadedEquipments.Accessories2Wearable.VariantId);
    FWearableInfo Acc3Wearable = GrivenkaSingleton->GetWearableInfo(this->LoadedEquipments.Accessories3Wearable.VariantId);

    FBodyTypeData BodyTypeData = GrivenkaSingleton->GetBodyTypeData(Gender);
    FBodyInfo Head = BodyTypeData.Head;
    FBodyInfo Torso = BodyTypeData.Torso;
    FBodyInfo Legs = BodyTypeData.Legs;
    FBodyInfo Hands = BodyTypeData.Hands;
    FBodyInfo Foot = BodyTypeData.Feet;

    if (!this->LoadedAppearance.SkinColorId.IsNone()) {
        FSkinColor SkinColor = GrivenkaSingleton->GetSkinColor(this->LoadedAppearance.SkinColorId);
        this->MaterialIns->SetVectorParameterValue("SkinColorMain", SkinColor.MainSkinColor);
        this->MaterialIns->SetVectorParameterValue("SkinColorSecondary", SkinColor.SecondarySkinColor);
        this->MaterialIns->SetVectorParameterValue("SkinColorTertiary", SkinColor.TertiarySkinColor);
        this->MaterialIns->SetVectorParameterValue("OutlineColor", SkinColor.OutlineSkinColor);
    }

    //Set Body Textures
    if (!Head.BodyId.IsNone()) {
        this->ModularParts.Partitions.FindRef("Head")->SetSkeletalMesh(Head.Mesh);
        if (!this->LoadedAppearance.HeadSkinId.IsNone()) this->MaterialIns->SetTextureParameterValue("Head", Head.TextureVariants.FindRef(this->LoadedAppearance.HeadSkinId));
        else this->MaterialIns->SetTextureParameterValue("Head", Head.TextureVariants.FindRef("Default"));
        if (!this->LoadedAppearance.EyeSkinId.IsNone()) this->MaterialIns->SetTextureParameterValue("Eyes", Head.TextureVariants.FindRef(this->LoadedAppearance.EyeSkinId));
        else this->MaterialIns->SetTextureParameterValue("Eyes", Head.TextureVariants.FindRef("Eye_Default"));
        if (!this->LoadedAppearance.MouthSkinId.IsNone()) this->MaterialIns->SetTextureParameterValue("Mouth", Head.TextureVariants.FindRef(this->LoadedAppearance.MouthSkinId));
        else this->MaterialIns->SetTextureParameterValue("Mouth", Head.TextureVariants.FindRef("Mouth_Default"));
        if (!this->LoadedAppearance.LPupilSkinId.IsNone()) this->MaterialIns->SetTextureParameterValue("Pupil_L", Head.TextureVariants.FindRef(this->LoadedAppearance.LPupilSkinId));
        else this->MaterialIns->SetTextureParameterValue("Pupil_L", Head.TextureVariants.FindRef("Pupil_Default"));
        if (!this->LoadedAppearance.RPupilSkinId.IsNone()) this->MaterialIns->SetTextureParameterValue("Pupil_R", Head.TextureVariants.FindRef(this->LoadedAppearance.RPupilSkinId));
        else this->MaterialIns->SetTextureParameterValue("Pupil_R", Head.TextureVariants.FindRef("Pupil_Default"));
    }

    if (!this->LoadedAppearance.HairId.IsNone()) {
        FBodyInfo Hair = GrivenkaSingleton->GetBodyInfo(this->LoadedAppearance.HairId);
        this->ModularParts.Partitions.FindRef("Hair")->SetSkeletalMesh(Hair.Mesh);
        this->MaterialIns->SetTextureParameterValue("Hair", Hair.TextureVariants.FindRef("Default"));
    }

    if (!this->LoadedAppearance.FacialHairId.IsNone()) {
        FBodyInfo FacialHair = GrivenkaSingleton->GetBodyInfo(this->LoadedAppearance.FacialHairId);
        this->ModularParts.Partitions.FindRef("FacHair")->SetSkeletalMesh(FacialHair.Mesh);
        this->MaterialIns->SetTextureParameterValue("FacHair", FacialHair.TextureVariants.FindRef("Default"));
    }



    if (!Torso.BodyId.IsNone()) {
        //sponge: fat muscular textureId
        this->MaterialIns->SetTextureParameterValue("Torso", Torso.TextureVariants.FindRef("Default"));
    }

    if (!Hands.BodyId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Hands", Hands.TextureVariants.FindRef("Default"));
    }

    if (!Legs.BodyId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Legs", Legs.TextureVariants.FindRef("Default"));
    }

    if (!Foot.BodyId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Feet", Foot.TextureVariants.FindRef("Default"));
    }

    if (!HeadWearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("HeadWear", HeadWearable.TextureSlots[0]);
        this->ModularParts.Partitions.FindRef("HeadWear")->SetSkeletalMesh(HeadWearable.GeneralInfo.EquipmentMesh);
    }


    if (!OuterTorsoWearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("OuterShirt", OuterTorsoWearable.TextureSlots[0]);
        this->ModularParts.Partitions.FindRef("Torso")->SetSkeletalMesh(OuterTorsoWearable.GeneralInfo.EquipmentMesh);
        if (!TorsoWearable.GeneralInfo.EquipmentId.IsNone()) {
            this->MaterialIns->SetTextureParameterValue("Shirt", TorsoWearable.TextureSlots[0]);
        }
    }
    else if (!TorsoWearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Shirt", TorsoWearable.TextureSlots[0]);
        this->ModularParts.Partitions.FindRef("Torso")->SetSkeletalMesh(TorsoWearable.GeneralInfo.EquipmentMesh);
    }
    else {
        this->ModularParts.Partitions.FindRef("Torso")->SetSkeletalMesh(Torso.Mesh);
    }

    if (!LegsWearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Pants", LegsWearable.TextureSlots[0]);
        this->ModularParts.Partitions.FindRef("Legs")->SetSkeletalMesh(LegsWearable.GeneralInfo.EquipmentMesh);
    }
    else {
        this->ModularParts.Partitions.FindRef("Legs")->SetSkeletalMesh(Legs.Mesh);
    }

    if (!HandsWearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Gloves", HandsWearable.TextureSlots[0]);
        this->ModularParts.Partitions.FindRef("Hands")->SetSkeletalMesh(HandsWearable.GeneralInfo.EquipmentMesh);
    }
    else {
        this->ModularParts.Partitions.FindRef("Hands")->SetSkeletalMesh(Hands.Mesh);
    }

    if (!FootWearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Shoes", FootWearable.TextureSlots[0]);
        this->ModularParts.Partitions.FindRef("Feet")->SetSkeletalMesh(FootWearable.GeneralInfo.EquipmentMesh);
    }
    else {
        this->ModularParts.Partitions.FindRef("Feet")->SetSkeletalMesh(Foot.Mesh);
    }

    if (!Acc1Wearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Acc_Head", Acc1Wearable.TextureSlots[0]);
        this->ModularParts.Partitions.FindRef("Acc_Head")->SetSkeletalMesh(Acc1Wearable.GeneralInfo.EquipmentMesh);
    }

    if (!Acc2Wearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Acc_Body", Acc2Wearable.TextureSlots[0]);
        this->ModularParts.Partitions.FindRef("Acc_Body")->SetSkeletalMesh(Acc2Wearable.GeneralInfo.EquipmentMesh);
    }

    if (!Acc3Wearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Acc_Legs", Acc3Wearable.TextureSlots[0]);
        this->ModularParts.Partitions.FindRef("Acc_Legs")->SetSkeletalMesh(Acc3Wearable.GeneralInfo.EquipmentMesh);
    }

}

void UModularAppearanceSystem::ApplyAndBakeMorphTargets()
{   
    this->FinishedBakeCounter = 0;
    TArray<FName> PartitionKey;
    this->ModularParts.Partitions.GetKeys(PartitionKey);
    TArray<FMorphMatchData> BulkMatch;
    FOnMorphBakeFinishedVoid OnBakeFinished;
    OnBakeFinished.BindDynamic(this, &UModularAppearanceSystem::OnBakeFinished);
    TArray<FName> MorphTargetKeys;
    this->LoadedAppearance.ShapeKeyValues.GetKeys(MorphTargetKeys);

    for (int i = 0; i < PartitionKey.Num(); i++) {
        auto Partition = this->ModularParts.Partitions.FindRef(PartitionKey[i]);

        for (int j = 0; j < MorphTargetKeys.Num(); j++) {
            float MorphValue = this->LoadedAppearance.ShapeKeyValues.FindRef(MorphTargetKeys[j]);
            Partition->SetMorphTarget(MorphTargetKeys[j], MorphValue, false);
        }

        UMorphToolsFunctions::BakeMorphs(this->GetWorld(), Partition, BulkMatch, OnBakeFinished, true);
    }

}

void UModularAppearanceSystem::OnBakeFinished()
{
    this->FinishedBakeCounter++;
    TArray<FName> ModularPartsKeys;
     this->ModularParts.Partitions.GetKeys(ModularPartsKeys);
     if (FinishedBakeCounter < ModularPartsKeys.Num()) return;
     //this->ComputeNormals(this->CompContext->SkeletalMeshComp->SkeletalMesh);

     UGrivenkaDataSingleton* GrivenkaSingleton = UGrivenkaSingletonLibrary::GetGrivenkaData();
     auto BodyTypeData = GrivenkaSingleton->GetBodyTypeData(this->Gender);
     
     FSkeletalMeshMergeParams MergeParams;
     TArray<USkeletalMesh*> MeshesToMerge;
     
     TArray<FName> PartitionKey;
     this->ModularParts.Partitions.GetKeys(PartitionKey);
     MergeParams.Skeleton = BodyTypeData.Skeleton;

     for (int i = 0; i < PartitionKey.Num(); i++) {
         MergeParams.MeshesToMerge.Add(this->ModularParts.Partitions.FindRef(PartitionKey[i])->SkeletalMesh);
     }
  
     auto MergedMesh = this->MergeMeshes(MergeParams);
     this->CompContext->SkeletalMeshComp->SetSkeletalMesh(MergedMesh);
     this->CompContext->SkeletalMeshComp->SetMaterial(0, this->MaterialIns);
     this->CompContext->CharacterAnim = this->CompContext->SkeletalMeshComp->GetAnimInstance();
     this->OnMergeFinished();
}

/*
void UModularAppearanceSystem::OnCopyMorphFinished()
{
    //Apply morph targets on copied (unbaked) morphs
   TArray<FName> MorphTargetKeys;
   this->LoadedAppearance.ShapeKeyValues.GetKeys(MorphTargetKeys);
   for (int i = 0; i < MorphTargetKeys.Num(); i++) {
       float MorphValue = this->LoadedAppearance.ShapeKeyValues.FindRef(MorphTargetKeys[i]);
       this->CompContext->SkeletalMeshComp->SetMorphTarget(MorphTargetKeys[i], MorphValue, false);
   }
}
*/
/*
void UModularAppearanceSystem::ComputeNormals(USkeletalMesh* Mesh)
{

    auto TransferMesh = this->ModularParts.TempBakedMesh->SkeletalMesh;
    FSkeletalMeshLODRenderData* Model = &Mesh->GetResourceForRendering()->LODRenderData[0];
    FSkeletalMeshLODRenderData* Model2 = &TransferMesh->GetResourceForRendering()->LODRenderData[0];

    int32 ModelVertNum = Model->StaticVertexBuffers.PositionVertexBuffer.GetNumVertices();

    TArray<FVector> Positions;
    TArray<int32> Triangles;
    TArray<FVector2D> uvs;

    for (int32 i = 0; i < ModelVertNum; i++)
    {
        Positions.Add(Model2->StaticVertexBuffers.PositionVertexBuffer.VertexPosition(i));
        uvs.Add(Model2->StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(i, 0));
    }

    FRawStaticIndexBuffer16or32Interface& indexBuffer = *(Model2->MultiSizeIndexContainer.GetIndexBuffer());

    for (int32 i = 0; i < indexBuffer.Num(); i += 3)
    {
        Triangles.Add(indexBuffer.Get(i));
        Triangles.Add(indexBuffer.Get(i + 1));
        Triangles.Add(indexBuffer.Get(i + 2));
    }
    TArray<FVector> OutNormals;
    TArray<FProcMeshTangent> OutTangents;
    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Positions, Triangles, uvs, OutNormals, OutTangents);

    for (int32 i = 0; i < ModelVertNum; i++)
    {

        FVector TanX = OutTangents[i].TangentX;
        FPackedNormal TangentZ = OutNormals[i];
        //TangentZ.Vector.W = 255;
        FVector TanZ = TangentZ.ToFVector();

        FVector TanY = (TanZ ^ TanX) * ((float)TangentZ.Vector.W / 127.5f - 1.0f);

        Model->StaticVertexBuffers.StaticMeshVertexBuffer.SetVertexTangents(
            i,
            TanX,
            TanY,
            OutNormals[i]
        );
    }
}
*/




