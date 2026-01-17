// Shungen All Rights Reserved.


#include "DataAssets/Item/ItemData.h"

/**
 * アイテム使用時の Niagara エフェクトを取得する。
 *
 * - UseEffect は TSoftObjectPtr で保持されているため、
 *   未ロードの場合はここで同期ロードを行う
 * - エフェクトが未設定（Null）の場合は nullptr を返す
 *
 * @return 使用時エフェクト。未設定の場合は nullptr
 */
UNiagaraSystem* UItemData::GetUseEffect()
{
    // DataAsset 上で未設定の場合
    if (UseEffect.IsNull())
    {
        return nullptr;
    }

    // 参照はあるが未ロードの場合は同期ロード
    if (!UseEffect.IsValid())
    {
        return UseEffect.LoadSynchronous();
    }

    // すでにロード済みの場合
    return UseEffect.Get();
}

/**
 * アイテム使用時のサウンドを取得する。
 *
 * - UseSound は TSoftObjectPtr で保持されており、
 *   必要になったタイミングでロードされる設計
 * - UI / Gameplay 側はロード状態を意識せず、この関数を呼ぶだけでよい
 *
 * @return 使用時サウンド。未設定の場合は nullptr
 */
USoundBase* UItemData::GetUseSound()
{
    // DataAsset 上で未設定の場合
    if (UseSound.IsNull())
    {
        return nullptr;
    }

    // 未ロードであれば同期ロード
    if (!UseSound.IsValid())
    {
        return UseSound.LoadSynchronous();
    }

    // すでにロード済み
    return UseSound.Get();
}

