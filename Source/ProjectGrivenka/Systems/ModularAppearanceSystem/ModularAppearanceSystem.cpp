// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularAppearanceSystem.h"
#include "SkeletalMeshMerge.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
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

void UModularAppearanceSystem::InitiateAppearance(FPersistedCharacterAppearance InAppearance, FPersistedEquipments InEquipments, int InGender)
{
    //sponge: need to do hiddenby/replacedby
    UGrivenkaDataSingleton* GrivenkaSingleton = UGrivenkaSingletonLibrary::GetGrivenkaData();
    FWearableInfo HeadWearable = GrivenkaSingleton->GetWearableInfo(InEquipments.HeadWearable.VariantId);
    FWearableInfo OuterTorsoWearable = GrivenkaSingleton->GetWearableInfo(InEquipments.OuterTorsoWearable.VariantId);
    FWearableInfo TorsoWearable = GrivenkaSingleton->GetWearableInfo(InEquipments.TorsoWearable.VariantId);
    FWearableInfo LegsWearable = GrivenkaSingleton->GetWearableInfo(InEquipments.LegsWearable.VariantId);
    FWearableInfo HandsWearable = GrivenkaSingleton->GetWearableInfo(InEquipments.HandsWearable.VariantId);
    FWearableInfo FootWearable = GrivenkaSingleton->GetWearableInfo(InEquipments.FootWearable.VariantId);
    FWearableInfo Acc1Wearable = GrivenkaSingleton->GetWearableInfo(InEquipments.Accessories1Wearable.VariantId);
    FWearableInfo Acc2Wearable = GrivenkaSingleton->GetWearableInfo(InEquipments.Accessories2Wearable.VariantId);
    FWearableInfo Acc3Wearable = GrivenkaSingleton->GetWearableInfo(InEquipments.Accessories3Wearable.VariantId);

    //sponge: static id bad
    FName HeadId = InGender == 0 ? "MALE_HEAD_DEFAULT" : "FEMALE_HEAD_DEFAULT";
    FName TorsoId = InGender == 0 ? "MALE_TORSO_DEFAULT" : "FEMALE_TORSO_DEFAULT";
    FName LegsId = InGender == 0 ? "MALE_LEGS_DEFAULT" : "FEMALE_LEGS_DEFAULT";
    FName HandsId = InGender == 0 ? "MALE_HANDS_DEFAULT" : "FEMALE_HANDS_DEFAULT";
    FName FootId = InGender == 0 ? "MALE_FOOT_DEFAULT" : "FEMALE_FOOT_DEFAULT";
    FBodyInfo Head = GrivenkaSingleton->GetBodyInfo(HeadId);
    FBodyInfo Torso = GrivenkaSingleton->GetBodyInfo(TorsoId);
    FBodyInfo Legs = GrivenkaSingleton->GetBodyInfo(LegsId);
    FBodyInfo Hands = GrivenkaSingleton->GetBodyInfo(HandsId);
    FBodyInfo Foot = GrivenkaSingleton->GetBodyInfo(FootId);

    if (!InAppearance.SkinColorId.IsNone()) {
        FSkinColor SkinColor = GrivenkaSingleton->GetSkinColor(InAppearance.SkinColorId);
        this->MaterialIns->SetVectorParameterValue("SkinColorMain", SkinColor.MainSkinColor);
        this->MaterialIns->SetVectorParameterValue("SkinColorSecondary", SkinColor.SecondarySkinColor);
        this->MaterialIns->SetVectorParameterValue("SkinColorTertiary", SkinColor.TertiarySkinColor);
        this->MaterialIns->SetVectorParameterValue("OutlineColor", SkinColor.OutlineSkinColor);
    }

    if (!InAppearance.HairId.IsNone()) {
        FBodyInfo Hair = GrivenkaSingleton->GetBodyInfo(InAppearance.HairId);
        this->ModularParts.Hair->SetSkeletalMesh(Hair.Mesh);
        this->MaterialIns->SetTextureParameterValue("Hair", Hair.TextureVariants.FindRef("Default"));
    }

    if (!InAppearance.FacialHairId.IsNone()) {
        FBodyInfo FacialHair = GrivenkaSingleton->GetBodyInfo(InAppearance.FacialHairId);
        this->ModularParts.Hair->SetSkeletalMesh(FacialHair.Mesh);
        this->MaterialIns->SetTextureParameterValue("FacHair", FacialHair.TextureVariants.FindRef("Default"));
    }

    if (!Head.BodyId.IsNone()) {
        this->CompContext->SkeletalMeshComp->SetSkeletalMesh(Head.Mesh);
        if (!InAppearance.HeadSkinId.IsNone()) this->MaterialIns->SetTextureParameterValue("Head", Head.TextureVariants.FindRef(InAppearance.HeadSkinId));
        else this->MaterialIns->SetTextureParameterValue("Head", Head.TextureVariants.FindRef("Default"));
        if (!InAppearance.EyeSkinId.IsNone()) this->MaterialIns->SetTextureParameterValue("Eyes", Head.TextureVariants.FindRef(InAppearance.EyeSkinId));
        else this->MaterialIns->SetTextureParameterValue("Eyes", Head.TextureVariants.FindRef("Eye_Default"));
        if (!InAppearance.MouthSkinId.IsNone()) this->MaterialIns->SetTextureParameterValue("Mouth", Head.TextureVariants.FindRef(InAppearance.MouthSkinId));
        else this->MaterialIns->SetTextureParameterValue("Mouth", Head.TextureVariants.FindRef("Mouth_Default"));
        if (!InAppearance.LPupilSkinId.IsNone()) this->MaterialIns->SetTextureParameterValue("Pupil_L", Head.TextureVariants.FindRef(InAppearance.LPupilSkinId));
        else this->MaterialIns->SetTextureParameterValue("Pupil_L", Head.TextureVariants.FindRef("Pupil_Default"));
        if (!InAppearance.RPupilSkinId.IsNone()) this->MaterialIns->SetTextureParameterValue("Pupil_R", Head.TextureVariants.FindRef(InAppearance.RPupilSkinId));
        else this->MaterialIns->SetTextureParameterValue("Pupil_R", Head.TextureVariants.FindRef("Pupil_Default"));
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
        this->ModularParts.HeadWear->SetSkeletalMesh(HeadWearable.GeneralInfo.EquipmentMesh);
    }


    if (!OuterTorsoWearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("OuterShirt", OuterTorsoWearable.TextureSlots[0]);
        this->ModularParts.HeadWear->SetSkeletalMesh(OuterTorsoWearable.GeneralInfo.EquipmentMesh);
    }

    if (!TorsoWearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Shirt", TorsoWearable.TextureSlots[0]);
        this->ModularParts.HeadWear->SetSkeletalMesh(TorsoWearable.GeneralInfo.EquipmentMesh);
    }
    else {
        this->ModularParts.Torso->SetSkeletalMesh(Torso.Mesh);
    }

    if (!LegsWearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Pants", LegsWearable.TextureSlots[0]);
        this->ModularParts.HeadWear->SetSkeletalMesh(LegsWearable.GeneralInfo.EquipmentMesh);
    }
    else {
        this->ModularParts.Legs->SetSkeletalMesh(Legs.Mesh);
    }

    if (!HandsWearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Gloves", HandsWearable.TextureSlots[0]);
        this->ModularParts.HeadWear->SetSkeletalMesh(HandsWearable.GeneralInfo.EquipmentMesh);
    }
    else {
        this->ModularParts.Hands->SetSkeletalMesh(Hands.Mesh);
    }

    if (!FootWearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Shoes", FootWearable.TextureSlots[0]);
        this->ModularParts.HeadWear->SetSkeletalMesh(FootWearable.GeneralInfo.EquipmentMesh);
    }
    else {
        this->ModularParts.Feet->SetSkeletalMesh(Foot.Mesh);
    }

    if (!Acc1Wearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Acc_Head", Acc1Wearable.TextureSlots[0]);
        this->ModularParts.HeadWear->SetSkeletalMesh(Acc1Wearable.GeneralInfo.EquipmentMesh);
    }

    if (!Acc2Wearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Acc_Body", Acc2Wearable.TextureSlots[0]);
        this->ModularParts.HeadWear->SetSkeletalMesh(Acc2Wearable.GeneralInfo.EquipmentMesh);
    }

    if (!Acc3Wearable.GeneralInfo.EquipmentId.IsNone()) {
        this->MaterialIns->SetTextureParameterValue("Acc_Legs", Acc3Wearable.TextureSlots[0]);
        this->ModularParts.HeadWear->SetSkeletalMesh(Acc3Wearable.GeneralInfo.EquipmentMesh);
    }

}
