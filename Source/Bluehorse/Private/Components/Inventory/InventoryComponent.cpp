// Shungen All Rights Reserved.


#include "Components/Inventory/InventoryComponent.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "DataAssets/Item/ItemData.h"
#include "AbilitySystemBlueprintLibrary.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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

void UInventoryComponent::AddItem(FGameplayTag ItemTag, int32 Amount, EItemType ItemType, int32 MaxStack, UItemData* ItemData)
{
	if (Amount <= 0) return;

	if (FInventorySlot* Slot = FindSlot(ItemTag))
	{
		Slot->Count = FMath::Clamp(Slot->Count + Amount, 0, Slot->MaxStack);
		return;
	}

	for (FInventorySlot& Slot : InventorySlots)
	{
		if (Slot.Count <= 0 && !Slot.ItemData)
		{
			FInventorySlot NewSlot;
			NewSlot.ItemTag = ItemTag;
			NewSlot.ItemType = ItemType;
			NewSlot.MaxStack = MaxStack;
			NewSlot.Count = FMath::Clamp(Amount, 0, MaxStack);
			NewSlot.ItemData = ItemData;
			NewSlot.ItemId = ItemData->GetPrimaryAssetId();

			Slot = NewSlot;
			return;
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

	FInventorySlot* Slot = &InventorySlots[CurrentInventoryIndex];

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

FInventorySlot& UInventoryComponent::GetCurrentItem()
{
	return InventorySlots[CurrentInventoryIndex];
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
			? Slot.ItemData->GetName()  // DataAsset–¼‚ðŽæ“¾
			: TEXT("None");

		FString TagName = Slot.ItemTag.ToString();

		UE_LOG(LogTemp, Log, TEXT("ItemTag: %s | ItemName: %s | Count: %d"),
			*TagName,
			*ItemName,
			Slot.Count);
	}

	UE_LOG(LogTemp, Log, TEXT("==========================="));
}
