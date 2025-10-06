// Shungen All Rights Reserved.


#include "AbilityTask/AbilityTask_WaitComboBuffer.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"

UAbilityTask_WaitComboBuffer::UAbilityTask_WaitComboBuffer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTestInitialState = true;
	bTriggerOnce = true;
}

// Ability発動中に追加の入力があったかのみを確認するAbilityTask
UAbilityTask_WaitComboBuffer* UAbilityTask_WaitComboBuffer::ConfirmInput(UGameplayAbility* OwningAbility, bool bTriggerOnce)
{
	UAbilityTask_WaitComboBuffer* Task = NewAbilityTask<UAbilityTask_WaitComboBuffer>(OwningAbility);
	Task->bTriggerOnce = bTriggerOnce;
	return Task;
}


void UAbilityTask_WaitComboBuffer::Activate()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	// ASCとAbilityの存在を確認し、無ければタスクを終了する
	if (!Ability || !ASC)
	{
		EndTask();
		return;
	}

	// 「Ability起動時から押しっぱなし」と「Ability発動者がローカルプレイヤー」かの確認
	// 主にマルチプレイでの判定に使うが、WaitInputPressと似たように実装している
	if (bTestInitialState && IsLocallyControlled())
	{
		if (FGameplayAbilitySpec* Spec = Ability->GetCurrentAbilitySpec())
		{
			if (Spec->InputPressed)
			{
				HandlePressed();
				if (bTriggerOnce) return;
			}
		}
	}
	Bind();
}

void UAbilityTask_WaitComboBuffer::OnDestroy(bool bInOwnerFinished)
{
	Unbind();
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitComboBuffer::HandlePressed()
{
	// 公式ドキュメントより
	// This should be called prior to broadcasting delegates back into the ability graph. This makes sure the ability is still active.
	// Abilityが途中でキャンセルされた場合などに無効なGAに対してBroadcastするのを防ぐ関所のようなもの
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnPressed.Broadcast();
	}

	if (bTriggerOnce)
	{
		Unbind();
		EndTask();
	}
}

void UAbilityTask_WaitComboBuffer::Bind()
{
	if (DelegateHandle.IsValid()) return;

	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		// AbilityReplicatedEventDelegateが追加入力を受け取る鍵
		// EAbilityGenericReplicatedEvent::InputPressed：追加入力に対してGameplayAbilityのロジック内で他の操作が実行される
		DelegateHandle = ASC->AbilityReplicatedEventDelegate(
			EAbilityGenericReplicatedEvent::InputPressed,
			GetAbilitySpecHandle(),
			GetActivationPredictionKey()
		).AddUFunction(this, FName(TEXT("HandlePressed")));
	}
}

void UAbilityTask_WaitComboBuffer::Unbind()
{
	if (!DelegateHandle.IsValid()) return;

	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		// ASCからこのタスクを削除する
		ASC->AbilityReplicatedEventDelegate(
			EAbilityGenericReplicatedEvent::InputPressed,
			GetAbilitySpecHandle(),
			GetActivationPredictionKey()
		).Remove(DelegateHandle);
	}
	DelegateHandle.Reset();
}


