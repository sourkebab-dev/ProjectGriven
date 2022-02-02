// Fill out your copyright notice in the Description page of Project Settings.


#include "UISmithTree.h"
#include "Components/GridPanel.h"
#include "ProjectGrivenka/Equipments/SmithingRecipeTreePrefab.h"
#include "ProjectGrivenka/Equipments/SmithingRecipePrefab.h"
#include "ProjectGrivenka/Equipments/WeaponPrefabs.h"
#include "ProjectGrivenka/Items/ItemPrefab.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/UI/Smithing/UISmithItem.h"

void UUISmithTree::Render(EEquipmentType InEquipmentType, EEquipmentTree InChosenEquipment, FGuid InEquipmentInsGuid, FName InEquipmentId)
{
	auto GI = Cast<UBaseGameInstance>(this->GetWorld()->GetGameInstance());
	FSmithingRecipeTreeItem CurrentRecipe;
	
	for (int i = 0; i < this->RecipeTree->SmithingRecipeTree.Num(); i++) {
		//sponge: need to check if its an armor (if i want an armor upgrade)
		if (this->RecipeTree->SmithingRecipeTree[i].Recipe->SmithingResultWeapon->WeaponInfo.GeneralInfo.EquipmentId == InEquipmentId) {
			CurrentRecipe = this->RecipeTree->SmithingRecipeTree[i];
			break;
		}
	}

	for (int i = 0; i < CurrentRecipe.ChildrenIndex.Num(); i++) {
		FSmithingRecipeTreeItem Child = this->RecipeTree->SmithingRecipeTree[CurrentRecipe.ChildrenIndex[i]];

		bool isChildSmithable = true;
		for (int j = 0; j < Child.Recipe->Recipe.Num(); j++) {
			if (!GI->CheckSmithingRequirements(Child.Recipe->Recipe[j].MaterialItem->ItemInfo, Child.Recipe->Recipe[j].Count)) {
				isChildSmithable = false;
				break;
			};
		}

		if (isChildSmithable) {
			auto UIIns = this->RecipeToUIMap.FindRef(Child.Recipe);
			if (UIIns) {
				UIIns->Enable();
			}
		}
	}
}
