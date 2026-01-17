// Shungen All Rights Reserved.


#include "GameModes/BluehorseMainGameMode.h"


/**
 * メインゲーム用 GameMode。
 *
 * - ゲーム全体の進行状態（MainGameModeState）を管理する
 * - 状態変更は Delegate 経由で通知し、UI や他システムと疎結合に連携する
 */
void ABluehorseMainGameMode::BeginPlay()
{
	// GameMode の初期化処理
	Super::BeginPlay();
}

/**
 * 毎フレーム呼ばれる Tick 処理。
 *
 * - 現時点では特別な処理は行っていない
 * - 将来的に「状態に応じた全体制御」や「デバッグ用途」などを追加できるよう残している
 */
void ABluehorseMainGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


/**
 * メインゲームの状態を変更する。
 *
 * - 内部状態を更新した後、OnMainGameModeStateChanged を Broadcast する
 * - UI や Manager クラスはこの Delegate を購読することで、
 *   GameMode の状態変化に追従できる
 *
 * @param InState 新しいメインゲーム状態
 */
void ABluehorseMainGameMode::SetCurrentMainGameModeState(EBluehorseMainGameModeState InState)
{
	// 現在の状態を更新
	CurrentMainGameModeState = InState;

	// 状態変更を通知
	OnMainGameModeStateChanged.Broadcast(CurrentMainGameModeState);
}
