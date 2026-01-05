// Shungen All Rights Reserved.


#include "AbilitySystem/Abilities/Attacks/GA_HeroAttackMaster.h"
#include "BluehorseFunctionLibrary.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "BluehorseTypes/BluehorseStructTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BluehorseGameplayTags.h"
#include "Characters/BluehorseHeroCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Ability終了時のタグ除去処理
void UGA_HeroAttackMaster::RemoveTagsOnEndAbility()
{
    AActor* Avatar = GetAvatarActorFromActorInfo();

    if (Avatar)
    {
        // Ability終了時に削除すべきタグ一覧を走査
        for (const FGameplayTag& Tag : TagsToRemoveOnEnd)
        {
            // アクターが指定タグを持っている場合のみ削除
            if (UBluehorseFunctionLibrary::NativeDoesActorHaveTag(Avatar, Tag))
            {
                UBluehorseFunctionLibrary::RemoveGameplayTagFromActorIfFound(Avatar, Tag);
            }
        }
    }
}

// 右手武器のダメージSpecHandleを生成
FGameplayEffectSpecHandle UGA_HeroAttackMaster::GetHeroEquippedRightHandWeaponDamageSpecHandle()
{
    int WeaponBaseDamage = GetHeroCombatComponentFromActorInfo()->GetHeroEquippedRightHandWeaponDamageAtLevel(GetAbilityLevel());

    return MakeHeroDamageEffectSpecHandle(EffectClass, WeaponBaseDamage, CurrentAttackTypeTag, GetMotionValueFromDataTable());
}

// 左手武器のダメージSpecHandleを生成
FGameplayEffectSpecHandle UGA_HeroAttackMaster::GetHeroEquippedLeftHandWeaponDamageSpecHandle()
{
    int WeaponBaseDamage = GetHeroCombatComponentFromActorInfo()->GetHeroEquippedLeftHandWeaponDamageAtLevel(GetAbilityLevel());

    return MakeHeroDamageEffectSpecHandle(EffectClass, WeaponBaseDamage, CurrentAttackTypeTag, GetMotionValueFromDataTable());
}

// ダメージを実際に適用する処理
void UGA_HeroAttackMaster::HandleApplyDamage(const FGameplayEventData& EventData)
{
    UE_LOG(LogTemp, Warning, TEXT("[HandleApplyDamage] Start"));

    // 対象アクターを取得（TargetはTObjectPtr<const AActor>なのでconst_castを使用）
    AActor* TargetActor = const_cast<AActor*>(EventData.Target.Get());

    UE_LOG(LogTemp, Warning, TEXT("[HandleApplyDamage] TargetActor: %s"),
        TargetActor ? *TargetActor->GetName() : TEXT("NULL"));

    if (!TargetActor)
    {
        UE_LOG(LogTemp, Error, TEXT("[HandleApplyDamage] TargetActor is NULL. Abort."));
        return;
    }

    // Ability / Owner 情報
    if (GetAvatarActorFromActorInfo())
    {
        UE_LOG(LogTemp, Warning, TEXT("[HandleApplyDamage] AvatarActor: %s"),
            *GetAvatarActorFromActorInfo()->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[HandleApplyDamage] AvatarActor is NULL"));
    }

    UE_LOG(LogTemp, Warning, TEXT("[HandleApplyDamage] IsRightWeapon: %s"),
        IsRightWeapon ? TEXT("true") : TEXT("false"));

    FActiveGameplayEffectHandle ActiveGameplayEffectHandle;

    // 使用している武器（右手／左手）に応じてSpecHandleを取得し、Effectを適用
    if (IsRightWeapon)
    {
        UE_LOG(LogTemp, Warning, TEXT("[HandleApplyDamage] Apply Right Weapon Damage Spec"));

        ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, GetHeroEquippedRightHandWeaponDamageSpecHandle());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[HandleApplyDamage] Apply Left Weapon Damage Spec"));
        ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, GetHeroEquippedLeftHandWeaponDamageSpecHandle());
    }

    UE_LOG(LogTemp, Warning, TEXT("[HandleApplyDamage] Effect Applied Result: %s"),
        ActiveGameplayEffectHandle.WasSuccessfullyApplied() ? TEXT("SUCCESS") : TEXT("FAILED"));

    // 成功時にヒットリアクションイベントを発火（GASイベント通知）
    if (ActiveGameplayEffectHandle.WasSuccessfullyApplied())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[HandleApplyDamage] Send HitReact GameplayEvent to %s"),
            *TargetActor->GetName());

        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
            TargetActor,
            BluehorseGameplayTags::Shared_Event_HitReact,
            EventData
        );
    }
    else
    {
        UE_LOG(LogTemp, Error,
            TEXT("[HandleApplyDamage] Damage Effect was NOT applied. HitReact will NOT be sent."));
    }

    UE_LOG(LogTemp, Warning, TEXT("[HandleApplyDamage] End"));
}

// モーション値（攻撃倍率など）をDataTableから取得
float UGA_HeroAttackMaster::GetMotionValueFromDataTable()
{
    const FHeroMotionDataRow* Row = MotionValueDataTable->FindRow<FHeroMotionDataRow>(
        AttackName,
        TEXT("ContextString")
    );

    if (!Row)
    {
        return 1.0f;
    }

    return Row->MotionValue;
}

// 攻撃方向を入力ベクトルから算出してMotionWarpingに反映
void UGA_HeroAttackMaster::ComputeAttackDirection()
{
    ABluehorseHeroCharacter* Hero = GetHeroCharacterFromActorInfo();
    if (!Hero)
    {
        return;
    }

    FVector2D LastInput2D(0.f, 0.f);

    // 最後の移動入力を取得（入力がない場合はゼロベクトル）
    if (Hero->GetCharacterMovement())
    {
        FVector InputVec = Hero->GetLastMovementInputVector();
        LastInput2D = FVector2D(InputVec.X, InputVec.Y);
    }

    // 入力方向を正規化（2D平面上）
    UKismetMathLibrary::Normalize2D(LastInput2D);
    FVector CachedAttackDirection = FVector(LastInput2D.X, LastInput2D.Y, 0.f);

    // 入力がほぼゼロの場合はキャラの前方方向を使用
    if (CachedAttackDirection.IsNearlyZero(0.0001f))
    {
        CachedAttackDirection = Hero->GetActorForwardVector();
    }

    // MotionWarpingコンポーネントを取得
    UMotionWarpingComponent* MotionWarpingComp = Hero->GetMotionWarpingComponent();
    if (!MotionWarpingComp)
    {
        return;
    }

    // 攻撃方向に基づく回転を生成
    const FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(CachedAttackDirection);

    // MotionWarpingのWarpターゲットを更新
    MotionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(NormalAttackWarpTargetName, FVector::ZeroVector, TargetRotation);
}

bool UGA_HeroAttackMaster::CanPerformFatalAttackByTag(AActor*& OutHitActor, float LineTraceDistance)
{
    // Abilityの実行者（＝プレイヤー）を取得
    AActor* Executer = GetAvatarActorFromActorInfo();
    if (!Executer)
    {
        return false;
    }

    UWorld* World = Executer->GetWorld();
    if (!World)
    {
        return false;
    }

    FVector ExecuterLocation = Executer->GetActorLocation();
    FVector ExecuterForward = Executer->GetActorForwardVector();
    FVector TraceEnd = ExecuterLocation + ExecuterForward * LineTraceDistance;

    // Trace設定
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Executer);

    FHitResult HitResult;

    const bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(
        World,
        ExecuterLocation,
        TraceEnd,
        ObjectTypes,
        false,
        ActorsToIgnore,
        EDrawDebugTrace::None,
        HitResult,
        true
    );

    if (bHit)
    {
        if (AActor* HitActor = HitResult.GetActor())
        {
            // 致命一撃可能タグを持つか確認
            if (UBluehorseFunctionLibrary::NativeDoesActorHaveTag(HitActor, BluehorseGameplayTags::Shared_Status_Knockdown))
            {
                OutHitActor = HitActor;
                return true;
            }
        }
    }

    return false;
}

// プレイヤーの攻撃時に「致命一撃可能」かを判定し、可能ならイベント送信とWarp位置設定を行う
void UGA_HeroAttackMaster::TryTriggerFatalAttackIfPossible(EBluehorseSuccessType& Result)
{
    // 成功状態を初期化（失敗をデフォルト）
    Result = EBluehorseSuccessType::Failed;

    // アビリティ実行者（プレイヤーキャラクター）を取得
    ABluehorseHeroCharacter* Hero = GetHeroCharacterFromActorInfo();
    if (!Hero)
    {
        // キャラクターが取得できない場合は処理中断
        return;
    }

    // MotionWarpingコンポーネントを取得
    UMotionWarpingComponent* MotionWarpingComp = Hero->GetMotionWarpingComponent();
    if (!MotionWarpingComp)
    {
        // 取得できない場合はWarp不可のため終了
        return;
    }

    // 致命一撃の判定対象となる敵アクター
    AActor* OutHitActor = nullptr;

    // --- 致命一撃可能判定 ---
    // 前方200cmの範囲内に「致命一撃可能タグ」を持つ敵がいればtrue
    if (CanPerformFatalAttackByTag(OutHitActor, 200.f))
    {
        // 致命一撃可能な敵が見つかった場合
        // GameplayEventデータを作成して対象の敵に送信
        FGameplayEventData EventData;
        EventData.Instigator = Hero; // 攻撃者（プレイヤー）
        EventData.Target = OutHitActor; // 対象（敵）
        EventData.EventTag = BluehorseGameplayTags::Shared_Event_Executed; // イベントタグ（「致命実行」）

        // GASイベント送信（BlueprintでのSendGameplayEventToActor相当）
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
            OutHitActor,
            BluehorseGameplayTags::Shared_Event_Executed,
            EventData
        );
    }
    else
    {
        // 敵がいない、または致命可能でない場合は処理中断
        return;
    }

    // --- Warp位置の算出 ---
    // 対象の現在位置と向きを取得
    const FVector OutHitActorLocation = OutHitActor->GetActorLocation();
    const FVector OutHitActorFwdVector = OutHitActor->GetActorForwardVector();

    // 敵の正面方向に少し離れた位置をWarp目標にする
    FVector TargetLocation = OutHitActorLocation + OutHitActorFwdVector * FVector(100.f, 100.f, 0.f);

    // プレイヤーを敵の正面に向けるため、前方向を反転して回転値を生成
    const FVector ReversedVector = OutHitActorFwdVector * -1.0f;
    const FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(ReversedVector);

    // プレイヤーを敵の方向に回転させる
    bool bSetActorRotation = Hero->SetActorRotation(TargetRotation);

    if (bSetActorRotation)
    {
        // 回転成功時は、MotionWarpingにWarpターゲットを設定
        MotionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(FatalAttackWarpTargetName, TargetLocation, TargetRotation);

        // 成功フラグを返す（Blueprint上ではSuccessピンにつながる）
        Result = EBluehorseSuccessType::Successful;
    }
    else
    {
        // 回転できなかった場合は失敗として終了
        return;
    }
}
