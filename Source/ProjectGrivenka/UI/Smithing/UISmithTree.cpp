// Fill out your copyright notice in the Description page of Project Settings.


#include "UISmithTree.h"
#include "ProjectGrivenka/Equipments/SmithingRecipeTreePrefab.h"
#include "ProjectGrivenka/Equipments/SmithingRecipePrefab.h"
#include "ProjectGrivenka/Equipments/WeaponPrefabs.h"
#include "ProjectGrivenka/Items/ItemPrefab.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/UI/Smithing/UISmithItem.h"

void UUISmithTree::Render(FName EquipmentId)
{
	auto GI = Cast<UBaseGameInstance>(this->GetWorld()->GetGameInstance());
	FSmithingRecipeTreeItem CurrentRecipe;
	for (int i = 0; i < this->RecipeTree->SmithingRecipeTree.Num(); i++) {
		if (!this->RecipeTree->SmithingRecipeTree[i].Recipe->SmithingBaseWeapon) return;
		//sponge: need to check if its an armor (if i want an armor upgrade)
		if (this->RecipeTree->SmithingRecipeTree[i].Recipe->SmithingBaseWeapon->WeaponInfo.GeneralInfo.EquipmentId == EquipmentId) {
			CurrentRecipe = this->RecipeTree->SmithingRecipeTree[i];
			break;
		}
	}

	if (CurrentRecipe.Recipe->Recipe.Num() == 0) {
		//Sponge: add warning validation
		return;
	}

	for (int i = 0; i < CurrentRecipe.ChildrenIndex.Num(); i++) {
		FSmithingRecipeTreeItem Child = this->RecipeTree->SmithingRecipeTree[i];

		bool isChildSmithable = true;
		for (int j = 0; j < Child.Recipe->Recipe.Num(); i++) {
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
