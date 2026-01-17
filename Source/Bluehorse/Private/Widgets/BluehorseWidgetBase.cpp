// Shungen All Rights Reserved.


#include "Widgets/BluehorseWidgetBase.h"
#include "Interfaces/PawnUIInterface.h"
#include "Kismet/GameplayStatics.h"

/**
 * Widget の初期化処理。
 *
 * - OwningPlayerPawn が IPawnUIInterface を実装している場合、
 *   HeroUIComponent を取得して Blueprint 側へ通知する
 */
void UBluehorseWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// プレイヤーPawnが UI インターフェースを実装しているか確認
	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(GetOwningPlayerPawn()))
	{
		// Hero 用 UIComponent を取得できた場合、BP に初期化通知
		if (UHeroUIComponent* HeroUIComponent = PawnUIInterface->GetHeroUIComponent())
		{
			BP_OnOwningHeroUIComponentInitialized(HeroUIComponent);
		}
	}
}

/**
 * UI 効果音を再生する共通関数。
 *
 * - Widget 側から簡単に UI サウンドを鳴らすためのユーティリティ
 * - 2D 再生のため空間音響の影響を受けない
 */
void UBluehorseWidgetBase::PlayUISound(USoundBase* SoundToPlay)
{
	if (SoundToPlay)
	{
		UGameplayStatics::PlaySound2D(this, SoundToPlay);
	}
}

/**
 * 敵生成時に表示される Widget の初期化処理。
 *
 * - OwningEnemyActor から EnemyUIComponent を取得
 * - 取得後、Blueprint 側に UIComponent を渡して初期化処理を委譲する
 *
 * @param OwningEnemyActor この Widget に対応する敵 Actor
 */
void UBluehorseWidgetBase::InitEnemyCreatedWidget(AActor* OwningEnemyActor)
{
	// 敵 Actor が UI インターフェースを実装しているか確認
	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(OwningEnemyActor))
	{
		UEnemyUIComponent* EnemyUIComponent = PawnUIInterface->GetEnemyUIComponent();

		// EnemyUIComponent が必須のため、取得できない場合はエラー
		checkf(EnemyUIComponent, TEXT("Failed to extrac an EnemyUIComponent form %s"), *OwningEnemyActor->GetActorNameOrLabel());

		// Blueprint 側で敵 UI の初期化を行う
		BP_OnOwningEnemyUIComponentInitialized(EnemyUIComponent);
	}
}
