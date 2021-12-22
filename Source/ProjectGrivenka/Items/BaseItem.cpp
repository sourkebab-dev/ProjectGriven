// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"


ABaseItem::ABaseItem() : AActor()
{

	USceneComponent* RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	this->RootComponent = RootSceneComponent;

	this->ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Item Mesh"));
	this->ItemMesh->SetupAttachment(RootComponent);
	this->ItemMesh->SetCollisionProfileName("NoCollision");

}
// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseItem::OnUse()
{
	GLog->Log("Use Item");
	GLog->Log(this->ItemInfo.ItemId.ToString());
}

void ABaseItem::LoadInfo(FItemInfo InItemInfo)
{
	this->ItemInfo = InItemInfo;
	if(this->ItemInfo.Mesh) this->ItemMesh->SetSkeletalMesh(this->ItemInfo.Mesh);
}


