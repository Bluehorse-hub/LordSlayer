// Shungen All Rights Reserved.


#include "AbilitySystem/Abilities/HeroGameplayAbility_Hajiki.h"
#include "AbilitySystem/Attribute/HajikiAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Characters/BluehorseHeroCharacter.h"
#include "Items/Weapons/BluehorseWeaponBase.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "BluehorseGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BluehorseFunctionLibrary.h"

#include "BluehorseDebugHelper.h"

UHeroGameplayAbility_Hajiki::UHeroGameplayAbility_Hajiki()
{
	// イベントを受け取る Trigger 設定
	FAbilityTriggerData Trigger;
	Trigger.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(Trigger);
}

void UHeroGameplayAbility_Hajiki::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 弾き（Hajiki）の使用回数を更新
	UpdateHajikiCount();

	// 現在の弾き回数に応じた再生モンタージュのインデックスを取得
	int32 AnimIndex = GetCurrentHajikiAnimIndexFromAttribute();

	// モンタージュ配列の範囲外であれば安全にアビリティを終了
	if (!HajikiMontages.IsValidIndex(AnimIndex))
	{
		Debug::Print("Invalid Montage Index");
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 再生する弾き用アニメーションモンタージュを取得
	UAnimMontage* Montage = HajikiMontages[AnimIndex];

	// モンタージュ再生用のAbilityTaskを生成
	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			Montage,
			1.0f,
			NAME_None,
			false,
			1.0f,
			0.0f,
			false
		);

	// モンタージュ完了時・中断時・キャンセル時のコールバックをバインド
	Task->OnCompleted.AddDynamic(this, &UHeroGameplayAbility_Hajiki::OnHajikiMontageCompleted);
	Task->OnInterrupted.AddDynamic(this, &UHeroGameplayAbility_Hajiki::OnHajikiMontageInterrupted);
	Task->OnCancelled.AddDynamic(this, &UHeroGameplayAbility_Hajiki::OnHajikiMontageCancelled);

	// AbilityTask を有効化し、モンタージュ再生を開始
	Task->ReadyForActivation();

	// 弾き成功時のエフェクトを発生させる
	SpawnHajikiEffectAtLocation();
}

void UHeroGameplayAbility_Hajiki::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UBluehorseFunctionLibrary::RemoveGameplayTagFromActorIfFound(GetHeroCharacterFromActorInfo(), BluehorseGameplayTags::Player_Status_Hajiki);

	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return;
	}

	if (!HajikiEffectClass)
	{
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

	// ===== Make Outgoing Gameplay Effect Spec =====
	FGameplayEffectSpecHandle SpecHandle =
		MakeOutgoingGameplayEffectSpec(HajikiEffectClass, 1.0f);

	if (!SpecHandle.IsValid())
	{
		return;
	}

	// ===== Apply Gameplay Effect Spec To Owner =====
	ApplyGameplayEffectSpecToOwner(
		Handle,
		ActorInfo,
		ActivationInfo,
		SpecHandle
	);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

int32 UHeroGameplayAbility_Hajiki::GetCurrentHajikiAnimIndexFromAttribute()
{
	// ActorInfoからAbilitySystemComponentを取得
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	// ASC が取得できない場合は安全側として0を返す
	if (!ASC)
	{
		return 0;
	}

	// 弾き（Hajiki）用のAttributeSetを取得
	auto* HajikiSet = const_cast<UHajikiAttributeSet*>(ASC->GetSet<UHajikiAttributeSet>());

	// AttributeSetが存在しない場合も0を返す
	if (!HajikiSet)
	{
		return 0;
	}

	// 現在設定されている弾きアニメーションのインデックスを取得
	int32 CurrentIndex = HajikiSet->GetCurrentHajikiAnimIndex();

	// 次に再生するインデックスを計算
	// モンタージュ数で割ることでループさせ、連続弾き時に同じアニメが続かないようにする
	int32 NextIndex = (CurrentIndex + 1) % HajikiMontages.Num();

	// 次回用として AttributeSet にインデックスを保存
	HajikiSet->SetCurrentHajikiAnimIndex(NextIndex);

	// 今回再生する弾きアニメーションのインデックスを返す
	return NextIndex;
}

FVector UHeroGameplayAbility_Hajiki::GetHajikiEffectLocation(FName InSocketName)
{
	// 現在キャラクターが装備している右手武器を取得
	ABluehorseWeaponBase* HeroWeapon = GetHeroCombatComponentFromActorInfo()->GetCharacterEquippedRightHandWeapon();

	// 武器が存在しない場合は無効な位置を返す
	if (!HeroWeapon)
	{
		return FVector();
	}

	// 武器のStaticMeshComponentを取得
	UStaticMeshComponent* WeaponMesh = HeroWeapon->GetWeaponMesh();

	// メッシュが取得できない場合も無効な位置を返す
	if (!WeaponMesh)
	{
		return FVector();
	}

	// 指定されたソケットのワールド座標の Transform を取得
	FTransform SocketTransform = WeaponMesh->GetSocketTransform(InSocketName, RTS_World);

	// 弾き（Hajiki）エフェクト生成用のワールド位置を返す
	return SocketTransform.GetLocation();
}

void UHeroGameplayAbility_Hajiki::SpawnHajikiEffectAtLocation()
{
	// 弾き（Hajiki）用のエフェクトが設定されていない場合は何もしない
	if (!HajikiEffect)
	{
		return;
	}
	
	// 武器ソケットから弾きエフェクトを生成するワールド座標を取得
	FVector SocketLocation = GetHajikiEffectLocation(SocketName);

	// 指定位置にNiagaraエフェクトを生成
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		HajikiEffect,
		SocketLocation,
		FRotator::ZeroRotator
	);

	// 効果音が設定されていない場合はエフェクトのみ再生して終了
	if (!HajikiSound)
	{
		return;
	}

	// 弾き成功時の効果音を同じ位置で再生
	UGameplayStatics::PlaySoundAtLocation(
		this,
		HajikiSound,
		SocketLocation
	);
}

void UHeroGameplayAbility_Hajiki::UpdateHajikiCount()
{
	// ActorInfoからAbilitySystemComponentを取得
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	// ASCが取得できない場合はエラーを出して処理を中断
	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("[Hajiki] ASC is NULL"));
		return;
	}

	// 弾き（Hajiki）用のAttributeSetを取得
	auto* HajikiSet = const_cast<UHajikiAttributeSet*>(ASC->GetSet<UHajikiAttributeSet>());

	// AttributeSet が存在しない場合もエラーを出して中断
	if (!HajikiSet)
	{
		UE_LOG(LogTemp, Error, TEXT("[Hajiki] HajikiAttributeSet is NULL"));
		return;
	}

	// 弾きの使用回数を加算
	HajikiSet->AddHajikiCount(1.0);

	const float AfterCount = HajikiSet->GetCurrentHajikiCount();
	UE_LOG(LogTemp, Warning, TEXT("[Hajiki] Count After : %.0f"), AfterCount);

	// 一定回数に到達した場合の確認用ログ
	// （将来的に強化状態付与や別アビリティ解放などつけたい）
	if (HajikiSet->GetCurrentHajikiCount() == CountForReinforcement)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Hajiki] count is 10"));
	}
}

void UHeroGameplayAbility_Hajiki::OnHajikiMontageCompleted()
{
	// 弾きアニメーションが正常に完了した場合、
	// アビリティを終了して次の行動に遷移可能な状態にする
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UHeroGameplayAbility_Hajiki::OnHajikiMontageInterrupted()
{
	// 他アクションや被弾などによりモンタージュが中断された場合も、
	// 状態不整合を防ぐためアビリティを明示的に終了する
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UHeroGameplayAbility_Hajiki::OnHajikiMontageCancelled()
{
	// 入力キャンセルや外部要因によってキャンセルされた場合も、
	// 同様にアビリティを終了して後処理を統一する
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
