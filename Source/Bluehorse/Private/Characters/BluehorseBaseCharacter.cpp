// Shungen All Rights Reserved.


#include "Characters/BluehorseBaseCharacter.h"
#include "AbilitySystem/BluehorseAbilitySystemComponent.h"
#include "AbilitySystem/BluehorseAttributeSet.h"
#include "MotionWarpingComponent.h"

// Sets default values
ABluehorseBaseCharacter::ABluehorseBaseCharacter()
{
	// この BaseCharacter 自体は Tick を使用しない設計
	// （必要な更新は Ability / Component / Anim などへ分離）
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// デカール受けを無効化（見た目/コスト都合。必要なら派生で有効化も可）
	GetMesh()->bReceivesDecals = false;

	// Gameplay Ability System（GAS）の中核：AbilitySystemComponent
	// Base に持たせて Player/Enemy で共通化する
	BluehorseAbilitySystemComponent = CreateDefaultSubobject<UBluehorseAbilitySystemComponent>(TEXT("BluehorseAbilitySystemComponent"));

	// GAS AttributeSet（HP/スタミナ等）
	// Base に持たせて共通の Attribute 管理を行う
	BluehorseAttributeSet = CreateDefaultSubobject<UBluehorseAttributeSet>(TEXT("BluehorseAttributeSet"));

	// Motion Warping（モンタージュ中の位置合わせなど）
	// ターゲット追従等で使用する想定
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}

UAbilitySystemComponent* ABluehorseBaseCharacter::GetAbilitySystemComponent() const
{
	return GetBluehorseAbilitySystemComponent();
}

// 戦闘コンポーネントの取得
// Base では未定義（nullptr）とし、Hero / Enemy 派生で実装して返す設計
UPawnCombatComponent* ABluehorseBaseCharacter::GetPawnCombatComponent() const
{
	return nullptr;
}

// UI コンポーネントの取得
// Base では未定義（nullptr）とし、派生で PawnUIComponent を返す設計
UPawnUIComponent* ABluehorseBaseCharacter::GetPawnUIComponent() const
{
	return nullptr;
}

UNiagaraSystem* ABluehorseBaseCharacter::GetHitEffect() const
{
	// ヒット時のデフォルトエフェクト（派生で差し替え可能）
	return DefaultHitEffect;
}

USoundBase* ABluehorseBaseCharacter::GetHitSound() const
{
	// ヒット時のデフォルトサウンド（派生で差し替え可能）
	return DefaultHitSound;
}

// Controller に所持されたタイミングで呼ばれる
void ABluehorseBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// GASの初期化処理
	if (BluehorseAbilitySystemComponent)
	{
		BluehorseAbilitySystemComponent->InitAbilityActorInfo(this, this);

		// 起動時に必要なデータ（能力付与・初期属性など）が設定されているかを開発中に検知する
		ensureMsgf(!CharacterStartUpData.IsNull(), TEXT("Forgot to assign start up data to %s"), *GetName());
	}
}

