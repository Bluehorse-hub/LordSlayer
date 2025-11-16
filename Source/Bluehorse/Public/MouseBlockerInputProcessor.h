// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"

/**
* ＜自分の勉強用＞
 * 【Slateとは何か？ / What is Slate?】
 * ----------------------------------------------------------------------------------------------------
 * Slate は Unreal Engine の「最下層 UI フレームワーク」であり、UMG の内部実装を支える本体です
 * UMG（Widget Blueprint / UUserWidget）は Slate のラッパーでしかなく、入力やフォーカス管理、
 * ウィジェットの描画処理などはすべて Slate が担当しています
 *
 * 【Slateの役割 / Responsibilities】
 * ・マウス / キーボード / ゲームパッド入力の一次処理
 * ・ウィジェット階層（SWidget）の描画とレイアウト
 * ・UMGに届く前の「UIイベント前処理」を提供
 * ・エディタの UI も 100% Slate で作られている
 *
 * 【Slateを直接触るメリット / Why use Slate directly?】
 * UMG では制御できない UI 挙動（フォーカス喪失、マウス干渉、複雑なイベント処理など）を
 * エンジンレベルで完全に制御できるようになる
 *
 * 
 * ＜クラスの説明＞
 * FMouseBlockerInputProcessor
 * ----------------------------------------------------------------------------------------------------
 * 【概要 / Overview】
 * Unreal Engine の最下層 UI フレームワーク「Slate」に直接介入し、
 * マウス入力（クリック・移動・スクロール）をすべて無効化するためのクラス
 * 
 * 【目的 / Purpose】
 * ・UMG がマウスフォーカスを奪うのを防ぎ、ゲームパッド UI 操作を安定させる
 * ・タイトル画面など「ゲームパッド専用UI」を実現する
 * 
 * 【使用される場所 / Usage】
 * BluehorseGameInstance::Init() の中で Slate に登録される
 */
class BLUEHORSE_API FMouseBlockerInputProcessor : public IInputProcessor
{
public:
	// -------- 必須追加（UE5.3?） ----------
	// IInputProcessor が純粋仮想関数を持つため、
	// これを実装しないとインスタンス化できない
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override
	{
		// 何も処理しなくてOK
	}
	// --------------------------------------

	// マウスクリック (押し) を完全に無効化
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		return true; // マウス入力を完全にブロック
	}

	// マウスクリック (離し) を無効化
	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		return true;
	}

	// マウスカーソル移動を無効化
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		return true;
	}

	// マウスホイール / ジェスチャー入力を無効化
	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& WheelEvent, const FPointerEvent* GestureEvent) override
	{
		return true;
	}

	// デバッグ名
	virtual const TCHAR* GetDebugName() const override
	{
		return TEXT("MouseBlocker");
	}
};
