// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/PawnUIComponent.h"
#include "HeroUIComponent.generated.h"

/**
 * 武器変更時の通知用 Delegate。
 *
 * @param SoftWeaponIcon 装備中武器のアイコン（Soft参照）
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedWeaponChangedDelegate, TSoftObjectPtr<UTexture2D>, SoftWeaponIcon);

/**
 * アイテムスロット内容が変化した際の通知用 Delegate。
 *
 * @param SoftItemIcon 対象アイテムのアイコン
 * @param ItemCount   スタック数
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemSlotChangedDelegate, TSoftObjectPtr<UTexture2D>, SoftItemIcon, int32, ItemCount);

/**
 * アイテムスロットが空になった際の通知用 Delegate。
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemSlotRemoveDelegate);

/**
 * アイテム消費時の通知用 Delegate。
 *
 * @param SoftItemIcon 消費したアイテムのアイコン
 * @param ItemCount   消費後の残数
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemConsumeDelegate, TSoftObjectPtr<UTexture2D>, SoftItemIcon, int32, ItemCount);

/**
 * アイテムスロット初期化時の通知用 Delegate。
 *
 * @param SoftItemIcon 初期表示用アイコン
 * @param ItemCount   初期スタック数
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemInitDelegate, TSoftObjectPtr<UTexture2D>, SoftItemIcon, int32, ItemCount);


/**
 * Hero 用 UI コンポーネント。
 *
 * - PawnUIComponent を継承し、Hero 固有の UI 更新イベントを追加
 * - AbilitySystem / Inventory / Combat などの状態変化を Delegate 経由で Widget に通知する
 * - UI 側はこの Component の Delegate を Bind するだけで状態同期が可能
 */
UCLASS()
class BLUEHORSE_API UHeroUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

public:
	/** スキルポイント（SP）割合変化通知（0.0 ～ 1.0） */
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangeDelegate OnCurrentSkillPointChanged; // 親クラスで宣言済みのDelegate

	/** 必殺技ゲージ（Ultimate）割合変化通知 */
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangeDelegate OnCurrentUltimatePointChanged;

	/** スタミナ割合変化通知 */
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangeDelegate OnCurrentStaminaChanged;

	/** 装備武器が変更された際の通知 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnEquippedWeaponChangedDelegate OnEquippedWeaponChanged;

	/** アイテムスロット内容が変更された際の通知 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnItemSlotChangedDelegate OnItemSlotChanged;

	/** アイテムスロットが空になった際の通知 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnItemSlotRemoveDelegate OnItemSlotRemove;

	/** アイテム消費時の通知 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnItemConsumeDelegate OnItemConsume;

	/** アイテムスロット初期化時の通知 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnItemInitDelegate OnItemSlotInit;
}; 
