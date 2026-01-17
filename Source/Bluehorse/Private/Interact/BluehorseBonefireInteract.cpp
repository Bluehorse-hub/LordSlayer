// Shungen All Rights Reserved.


#include "Interact/BluehorseBonefireInteract.h"

#include "BluehorseDebugHelper.h"

/**
 * 焚き火（Bonfire）用のインタラクトオブジェクト。
 *
 * - 見た目・当たり判定は BaseInteract を利用
 * - インタラクト処理は IInteractableInterface を実装して定義する
 */

ABluehorseBonefireInteract::ABluehorseBonefireInteract()
{
    // --- Fire Particle ----------------------------------------------------
    // 焚き火の炎エフェクト用パーティクルコンポーネント
    FireParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireParticleComponent"));
    FireParticleComponent->SetupAttachment(GetRootComponent());

    // パーティクルは常時表示したいため自動再生
    FireParticleComponent->bAutoActivate = true;

    // --- Fire Audio -------------------------------------------------------
    // 焚き火のループSE用オーディオコンポーネント
    FireAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FireAudioComponent"));
    FireAudioComponent->SetupAttachment(GetRootComponent());

    // サウンドは BeginPlay で明示的に制御するため AutoActivate は false
    FireAudioComponent->bAutoActivate = false;
}

void ABluehorseBonefireInteract::BeginPlay()
{
    Super::BeginPlay();

    // --- Particle Setup ---------------------------------------------------
    // 設定されているパーティクルアセットを適用して再生
    if (FireParticleSystem)
    {
        FireParticleComponent->SetTemplate(FireParticleSystem);
        FireParticleComponent->Activate(true);
    }

    // --- Audio Setup ------------------------------------------------------
    // 設定されているループサウンドを再生
    if (FireLoopSound)
    {
        FireAudioComponent->SetSound(FireLoopSound);
        FireAudioComponent->Play();
    }
}
