// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "PawnUIComponent.generated.h"

/**
 * 割合（0.0 ～ 1.0）の変化を通知する Delegate。
 *
 * - HP / スタミナ / ゲージ系UIなど、汎用的に使用可能
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPercentChangeDelegate, float, NewPercent);

/**
 * Pawn 共通 UI コンポーネントの基底クラス。
 *
 * - PawnExtensionComponentBase を継承し、Pawn 前提の安全な取得を保証
 * - Hero / Enemy どちらにも共通する UI 情報（HP など）を扱う
 * - 実際の UI 更新処理は Widget 側で行い、この Component は通知専用
 */
UCLASS()
class BLUEHORSE_API UPawnUIComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:
	/**
	 * 現在 HP の割合が変化した際の通知。
	 *
	 * - NewPercent は 0.0 ～ 1.0 の正規化された値
	 * - Widget 側で HPバーなどの更新に使用する
	 */
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangeDelegate OnCurrentHealthChanged;
	
};
