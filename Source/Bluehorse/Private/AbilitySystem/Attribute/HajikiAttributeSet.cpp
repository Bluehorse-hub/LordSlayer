// Shungen All Rights Reserved.


#include "AbilitySystem/Attribute/HajikiAttributeSet.h"

UHajikiAttributeSet::UHajikiAttributeSet()
{
	// 弾き（Hajiki）アニメーションの初期インデックス
	InitHajikiAnimIndex(1.f);

	// 弾きの使用回数（カウント）の初期化
	InitHajikiCount(0.f);
}

int32 UHajikiAttributeSet::GetCurrentHajikiAnimIndex() const
{
	// 現在の弾きアニメーションインデックスを取得
	return GetHajikiAnimIndex();
}

void UHajikiAttributeSet::SetCurrentHajikiAnimIndex(int32 InIndex)
{
	// 次回再生用の弾きアニメーションインデックスを設定
	SetHajikiAnimIndex(InIndex);
}

int32 UHajikiAttributeSet::GetCurrentHajikiCount()
{
	// 現在の弾き成功回数を取得
	return GetHajikiCount();
}

void UHajikiAttributeSet::AddHajikiCount(int32 Amount)
{
	// 弾き成功時に回数を加算
	SetHajikiCount(GetHajikiCount() + Amount);
}

void UHajikiAttributeSet::ResetHajikiCount()
{
	// 弾きカウントをリセット
	SetHajikiCount(0.f);
}
