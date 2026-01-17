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
	// インベントリは Tick 不要（イベント駆動で更新）
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	// スロット数を固定長で確保する（UI/保存/インデックス参照の安定化）
	Super::BeginPlay();
	InventorySlots.SetNum(MaxInventorySlot);
}

// 指定アイテム（ItemTag）の所持数を返す
int32 UInventoryComponent::GetItemCount(FGameplayTag ItemTag) const
{
	if (const FInventorySlot* Slot = ConstFindSlot(ItemTag))
	{
		return Slot->Count;
	}
	return 0;
}

/**
 * アイテムを追加する。
 *
 * - すでに同じ ItemTag のスロットがある場合はスタックに加算（MaxStack でクランプ）
 * - 無ければ空スロットを探して新規に入れる
 * - 空きが無ければ「満杯」をログに出して追加しない
 */
void UInventoryComponent::AddItem(FGameplayTag ItemTag, int32 Amount, UItemData* ItemData)
{
	UE_LOG(LogTemp, Warning, TEXT("[AddItem] InventorySlots.Num() = %d"), InventorySlots.Num());

	if (Amount <= 0) return;

	// 既存スタックがあればそこに加算
	if (FInventorySlot* Slot = FindSlot(ItemTag))
	{
		Slot->Count = FMath::Clamp(Slot->Count + Amount, 0, ItemData->MaxStack);
		return;
	}

	// 空スロットを探して新規追加
	for (FInventorySlot& Slot : InventorySlots)
	{
		// 空判定：Count が 0 以下 & ItemData 未設定
		if (Slot.Count <= 0 && !Slot.ItemData)
		{
			FInventorySlot NewSlot;
			NewSlot.ItemTag = ItemTag;
			NewSlot.Count = FMath::Clamp(Amount, 0, ItemData->MaxStack);
			NewSlot.ItemData = ItemData;

			// PrimaryAssetId を保存しておくと、後の識別/復元に使える
			NewSlot.ItemId = ItemData->GetPrimaryAssetId();

			Slot = NewSlot;
			return;
		}
		else
		{
			// デバッグ：空いていないスロット状況の確認用
			UE_LOG(LogTemp, Warning, TEXT("Slot is not empty: Count=%d, ItemData=%s"),
				Slot.Count,
				Slot.ItemData ? *Slot.ItemData->GetName() : TEXT("None"));
		}
	}

	// 空きが無い（満杯）
	UE_LOG(LogTemp, Warning, TEXT("Inventory full! Could not add %s"), *ItemTag.ToString());
}

/**
 * ItemTag を指定してアイテムを使用する。
 *
 * - Slot が存在し、Count>0 かつ ItemData がある場合のみ使用可能
 * - ItemData の EffectToApply（GameplayEffect）を Self に適用する
 * - 使用後に Count を減らし、0 以下ならスロットを初期化して削除扱い（bSlotRemoved=true）
 */
bool UInventoryComponent::UseItemByTag(FGameplayTag ItemTag, UBluehorseAbilitySystemComponent* AbilitySystemComponent, bool& bSlotRemoved)
{
	bSlotRemoved = false;

	FInventorySlot* Slot = FindSlot(ItemTag);

	if (!Slot || Slot->Count <= 0 || !Slot->ItemData)
	{
		return false;
	}

	// 使用時効果（GameplayEffect）が設定されている場合のみ適用
	if (TSubclassOf<UGameplayEffect> Effect = Slot->ItemData->EffectToApply)
	{
		UGameplayEffect* EffectCDO = Effect->GetDefaultObject<UGameplayEffect>();

		// アイテム効果を Self に適用（ローカル動作前提でもASCに統一して適用）
		AbilitySystemComponent->ApplyGameplayEffectToSelf(
			EffectCDO,
			Slot->ItemData->ApplyLevel,
			AbilitySystemComponent->MakeEffectContext()
		);

		// 消費
		Slot->Count--;

		// 0になったらスロットを空に戻す
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

/**
 * 現在選択中スロット（CurrentInventoryIndex）のアイテムを使用する。
 *
 * - Index が有効で、Count>0 & ItemData がある場合のみ使用可能
 * - 効果適用/消費/空スロット化の流れは UseItemByTag と同様
 */
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


/**
 * 次のアイテムを選択する（循環）。
 * - UI の「使用アイテム選択」などを想定
 */
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

/**
 * 現在選択中のスロット内容を返す。
 * - Index が無効なら空スロットを返す
 */
FInventorySlot UInventoryComponent::GetCurrentItem()
{
	if (InventorySlots.IsValidIndex(CurrentInventoryIndex))
	{
		return InventorySlots[CurrentInventoryIndex];
	}

	return FInventorySlot();
}

/**
 * アイテム使用音を 2D で再生する（UI/効果音用）。
 */
void UInventoryComponent::PlayUseItemSound(USoundBase* SoundToPlay)
{
	if (SoundToPlay)
	{
		UGameplayStatics::PlaySound2D(this, SoundToPlay);
	}
}

/**
 * インベントリ内容を GameInstance に保存する（簡易永続化）。
 *
 * - GameInstance はレベル遷移しても残るため、所持品を維持したい場合に使用する
 * - ここでは ItemTag / Count / SlotIndex をスナップショットとして保存する
 */
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

/**
 * GameInstance に保存されているインベントリを復元する。
 *
 * - スロット配列を初期化した上で、保存されていた SlotIndex に復元する
 * - ItemTag → ItemData は DataTable から引き直す（Soft参照対応）
 */
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

/**
 * 指定 ItemTag のスロットを検索して返す（非const）。
 * - 見つからなければ nullptr
 */
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

/**
 * 指定 ItemTag のスロットを検索して返す（const）。
 */
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

/**
 * ItemTag から ItemData（DataAsset）を取得する。
 *
 * - DataTable 内の Row を走査し、ItemTag が一致するものを探す
 * - Row が SoftObjectPtr の場合は、未ロードなら同期ロードして返す
 */
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
			// すでにロード済みなら Get、未ロードなら同期ロード
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

/**
 * デバッグ用：インベントリの中身をログ出力する。
 */
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
