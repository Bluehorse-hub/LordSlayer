// Shungen All Rights Reserved.


#include "Controllers/BluehorseHeroController.h"

/**
 * Hero 用 PlayerController。
 *
 * - GenericTeamId を実装し、AIController 側の敵味方判定に利用される
 * - 現在は Hero（プレイヤー）を TeamId = 0 として扱う
 * - EnemyController 側では TeamId の比較によって Hostile / Friendly を判定する設計
 */
ABluehorseHeroController::ABluehorseHeroController()
{
	// プレイヤー陣営の TeamId
	HeroTeamId = FGenericTeamId(0);
}

FGenericTeamId ABluehorseHeroController::GetGenericTeamId() const
{
	return HeroTeamId;
}
