// Shungen All Rights Reserved.


#include "Components/Inventory/InventoryComponent.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "DataAssets/Item/ItemData.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "BluehorseGameInstance.h"

#include "BluehorseDebugHelper.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	InventorySlots.SetNum(MaxInventorySlot);
}

int32 UInventoryComponent::GetItemCount(FGameplayTag ItemTag) const
{
	if (const FInventorySlot* Slot = ConstFindSlot(ItemTag))
	{
		return Slot->Count;
	}
	return 0;
}

void UInventoryComponent::AddItem(FGameplayTag ItemTag, int32 Amount, UItemData* ItemData)
{
	UE_LOG(LogTemp, Warning, TEXT("[AddItem] InventorySlots.Num() = %d"), InventorySlots.Num());

	if (Amount <= 0) return;

	if (FInventorySlot* Slot = FindSlot(ItemTag))
	{
		Slot->Count = FMath::Clamp(Slot->Count + Amount, 0, ItemData->MaxStack);
		return;
	}

	for (FInventorySlot& Slot : InventorySlots)
	{
		if (Slot.Count <= 0 && !Slot.ItemData)
		{
			FInventorySlot NewSlot;
			NewSlot.ItemTag = ItemTag;
			NewSlot.Count = FMath::Clamp(Amount, 0, ItemData->MaxStack);
			NewSlot.ItemData = ItemData;
			NewSlot.ItemId = ItemData->GetPrimaryAssetId();

			Slot = NewSlot;
			return;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Slot is not empty: Count=%d, ItemData=%s"),
				Slot.Count,
				Slot.ItemData ? *Slot.ItemData->GetName() : TEXT("None"));
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Inventory full! Could not add %s"), *ItemTag.ToString());
}

bool UInventoryComponent::UseItemByTag(FGameplayTag ItemTag, UBluehorseAbilitySystemComponent* AbilitySystemComponent, bool& bSlotRemoved)
{
	bSlotRemoved = false;

	FInventorySlot* Slot = FindSlot(ItemTag);

	if (!Slot || Slot->Count <= 0 || !Slot->ItemData)
	{
		return false;
	}

	if (TSubclassOf<UGameplayEffect> Effect = Slot->ItemData->EffectToApply)
	{
		UGameplayEffect* EffectCDO = Effect->GetDefaultObject<UGameplayEffect>();
		AbilitySystemComponent->ApplyGameplayEffectToSelf(
			EffectCDO,
			Slot->ItemData->ApplyLevel,
			AbilitySystemComponent->MakeEffectContext()
		);

		Slot->Count--;

		if (Slot->Count <= 0)
		{
			*Slot = FInventorySlot();
			bSlotRemoved = true;
		}
		LogInventory();
		return true;
	}
	LogInventory();
	return false;
}

bool UInventoryComponent::UseItemByIndex(UBluehorseAbilitySystemComponent* AbilitySystemComponent, bool& bSlotRemoved)
{
	bSlotRemoved = false;

	if (!InventorySlots.IsValidIndex(CurrentInventoryIndex))
	{
		return false;
	}

	FInventorySlot* Slot = &InventorySlots[CurrentInventoryIndex];

	if (Slot->Count <= 0 || !Slot->ItemData)
	{
		return false;
	}

	if (TSubclassOf<UGameplayEffect> Effect = Slot->ItemData->EffectToApply)
	{
		UGameplayEffect* EffectCDO = Effect->GetDefaultObject<UGameplayEffect>();
		AbilitySystemComponent->ApplyGameplayEffectToSelf(
			EffectCDO,
			Slot->ItemData->ApplyLevel,
			AbilitySystemComponent->MakeEffectContext()
		);

		Slot->Count--;

		if (Slot->Count <= 0)
		{
			*Slot = FInventorySlot();
			bSlotRemoved = true;
		}
		LogInventory();
		return true;
	}
	LogInventory();
	return false;
}

void UInventoryComponent::SelectItem()
{
	if (CurrentInventoryIndex >= InventorySlots.Num() - 1)
	{
		CurrentInventoryIndex = 0;
	}
	else
	{
		CurrentInventoryIndex += 1;
	}
}

FInventorySlot UInventoryComponent::GetCurrentItem()
{
	if (InventorySlots.IsValidIndex(CurrentInventoryIndex))
	{
		return InventorySlots[CurrentInventoryIndex];
	}

	return FInventorySlot();
}

void UInventoryComponent::PlayUseItemSound(USoundBase* SoundToPlay)
{
	if (SoundToPlay)
	{
		UGameplayStatics::PlaySound2D(this, SoundToPlay);
	}
}

void UInventoryComponent::SaveInventoryToGameInstance()
{
	if (UBluehorseGameInstance* GameInstance = GetWorld()->GetGameInstance<UBluehorseGameInstance>())
	{
		// 前回のデータをクリアする
		GameInstance->PersistentInventory.Empty();

		int Index = 0;

		for (const FInventorySlot& Slot : InventorySlots)
		{
			const FGameplayTag& ItemTag = Slot.ItemTag;
			const int32 Count = Slot.Count;

			FStoredItemData Snapshot;
			Snapshot.ItemTag = ItemTag;
			Snapshot.Count = Count;
			Snapshot.SlotIndex = Index;

			GameInstance->PersistentInventory.Add(Snapshot);

			Index += 1;
		}
	}
}

void UInventoryComponent::LoadInventoryFromGameInstance()
{
	if (UBluehorseGameInstance* GameInstance = GetWorld()->GetGameInstance<UBluehorseGameInstance>())
	{
		InventorySlots.Empty();
		InventorySlots.SetNum(MaxInventorySlot);

		for (const FStoredItemData& Data : GameInstance->PersistentInventory)
		{
			UItemData* ItemData = GetItemDataFromTag(Data.ItemTag);
			if (!ItemData)
			{
				continue;
			}

			FInventorySlot NewSlot;
			NewSlot.ItemData = ItemData;
			NewSlot.Count = Data.Count;
			NewSlot.ItemTag = Data.ItemTag;
			NewSlot.ItemId = ItemData->GetPrimaryAssetId();

			InventorySlots[Data.SlotIndex] = NewSlot;
		}
	}
}

FInventorySlot* UInventoryComponent::FindSlot(FGameplayTag ItemTag)
{
	for (FInventorySlot& Slot : InventorySlots)
	{
		if (Slot.ItemTag == ItemTag)
		{
			return &Slot;
		}
	}

	return nullptr;
}

const FInventorySlot* UInventoryComponent::ConstFindSlot(FGameplayTag ItemTag) const
{
	for (const FInventorySlot& Slot : InventorySlots)
	{
		if (Slot.ItemTag == ItemTag)
		{
			return &Slot;
		}
	}

	return nullptr;
}

UItemData* UInventoryComponent::GetItemDataFromTag(const FGameplayTag& ItemTag) const
{
	if (!ItemDataTable)
	{
		Debug::Print(TEXT("[Inventory] ItemDataTable is not set!"), FColor::Red);
		return nullptr;
	}

	static const FString ContextString(TEXT("Item Lookup"));
	TArray<FItemDataTableRow*> AllRows;
	ItemDataTable->GetAllRows(ContextString, AllRows);

	for (FItemDataTableRow* Row : AllRows)
	{
		if (Row && Row->ItemTag == ItemTag)
		{
			if (Row->ItemData.IsValid())
			{
				return Row->ItemData.Get();
			}
			else
			{
				return Row->ItemData.LoadSynchronous();
			}
		}
	}

	return nullptr;
}

void UInventoryComponent::LogInventory() const
{
	UE_LOG(LogTemp, Log, TEXT("===== Inventory Slots ====="));

	if (InventorySlots.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("Inventory is empty"));
	}

	for (const FInventorySlot& Slot : InventorySlots)
	{
		FString ItemName = Slot.ItemData
			? Slot.ItemData->GetName()  // DataAsset名を取得
			: TEXT("None");

		FString TagName = Slot.ItemTag.ToString();

		UE_LOG(LogTemp, Log, TEXT("ItemTag: %s | ItemName: %s | Count: %d"),
			*TagName,
			*ItemName,
			Slot.Count);
	}

	UE_LOG(LogTemp, Log, TEXT("==========================="));
}
