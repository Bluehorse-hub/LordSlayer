// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BluehorseHeroGameplayAbility.h"
#include "GA_HeroAOEUltimateMaster.generated.h"

/**
*
*/
UCLASS()
class BLUEHORSE_API UGA_HeroAOEUltimateMaster : public UBluehorseHeroGameplayAbility
{
	GENERATED_BODY()

	// 現時点では空クラス
	// 必殺技専用のGAとしてBlueprint側でAOE攻撃やシーケンサー演出を制御
	// 将来的にC++専用の必殺技ロジックを統合できるように構造を確保している
};
