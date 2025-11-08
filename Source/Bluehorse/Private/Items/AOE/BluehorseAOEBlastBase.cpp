// Shungen All Rights Reserved.


#include "Items/AOE/BluehorseAOEBlastBase.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BluehorseFunctionLibrary.h"
#include "BluehorseGameplayTags.h"

void ABluehorseAOEBlastBase::SpawnAOESphere_Implementation()
{
	Super::SpawnAOESphere_Implementation();

    FVector SpawnLocation = GetActorLocation();

	if (NiagaraEffect)
	{
        FRotator SpawnRotation = GetActorRotation();

        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            NiagaraEffect,
            SpawnLocation,
            SpawnRotation,
            FVector(1.0f),
            true,
            true,
            ENCPoolMethod::AutoRelease,
            true
        );
	}

    if (SpawnSound)
    {
        UAudioComponent* AudioComp = UGameplayStatics::SpawnSoundAtLocation(
            GetWorld(),
            SpawnSound,
            SpawnLocation,
            FRotator::ZeroRotator,
            1.0f, // Volume
            1.0f, // Pitch
            0.0f, // Start time
            nullptr, // Attenuation settings
            nullptr, // Concurrency settings
            true     // Auto destroy
        );
    }
    Destroy();
}

void ABluehorseAOEBlastBase::OnAOEOverlapActor_Implementation(AActor* OtherActor)
{
    Super::OnAOEOverlapActor_Implementation(OtherActor);

    // 衝突対象がPawnかどうかを確認
    APawn* HitPawn = Cast<APawn>(OtherActor);

    if (!HitPawn)
    {
        return;
    }

    // 敵対関係の判定
    // 敵対関係がある場合のみダメージ適用
    if (!UBluehorseFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
    {
        return;
    }

    // 無敵状態のキャラクターはダメージ無効
    const bool bIsPlayerInvincible = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitPawn, BluehorseGameplayTags::Player_Status_Invincible);

    // すり抜け動作になるように、そのまま終了
    if (bIsPlayerInvincible)
    {
        return;
    }

    bool bIsValidBlock = false;

    const bool bIsPlayerBlocking = UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitPawn, BluehorseGameplayTags::Player_Status_Blocking);

    if (bIsPlayerBlocking)
    {
        // Projectileの角度や位置などから「有効なブロック」かどうかを判定
        bIsValidBlock = UBluehorseFunctionLibrary::IsValidBlock(this, HitPawn);
    }

    // GameplayEvent用のデータを作成（Instigator: Projectile, Target: 被弾者）
    FGameplayEventData Data;
    Data.Instigator = this;
    Data.Target = HitPawn;

    // Projectileに紐づくGameplayEffectSpecHandleを使ってダメージを適用
    if (bIsValidBlock)
    {
        // ブロックが成立した場合、GASイベントを発火
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
            HitPawn,
            BluehorseGameplayTags::Player_Event_SuccessfulBlock,
            Data
        );
    }
    else
    {
        HandleApplyAOEDamage(HitPawn, Data);
    }
}
