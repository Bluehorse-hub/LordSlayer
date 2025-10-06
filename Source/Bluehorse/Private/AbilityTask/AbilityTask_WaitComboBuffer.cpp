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

// Ability�������ɒǉ��̓��͂����������݂̂��m�F����AbilityTask
UAbilityTask_WaitComboBuffer* UAbilityTask_WaitComboBuffer::ConfirmInput(UGameplayAbility* OwningAbility, bool bTriggerOnce)
{
	UAbilityTask_WaitComboBuffer* Task = NewAbilityTask<UAbilityTask_WaitComboBuffer>(OwningAbility);
	Task->bTriggerOnce = bTriggerOnce;
	return Task;
}


void UAbilityTask_WaitComboBuffer::Activate()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	// ASC��Ability�̑��݂��m�F���A������΃^�X�N���I������
	if (!Ability || !ASC)
	{
		EndTask();
		return;
	}

	// �uAbility�N�������牟�����ςȂ��v�ƁuAbility�����҂����[�J���v���C���[�v���̊m�F
	// ��Ƀ}���`�v���C�ł̔���Ɏg�����AWaitInputPress�Ǝ����悤�Ɏ������Ă���
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
	// �����h�L�������g���
	// This should be called prior to broadcasting delegates back into the ability graph. This makes sure the ability is still active.
	// Ability���r���ŃL�����Z�����ꂽ�ꍇ�Ȃǂɖ�����GA�ɑ΂���Broadcast����̂�h���֏��̂悤�Ȃ���
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
		// AbilityReplicatedEventDelegate���ǉ����͂��󂯎�錮
		// EAbilityGenericReplicatedEvent::InputPressed�F�ǉ����͂ɑ΂���GameplayAbility�̃��W�b�N���ő��̑��삪���s�����
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
		// ASC���炱�̃^�X�N���폜����
		ASC->AbilityReplicatedEventDelegate(
			EAbilityGenericReplicatedEvent::InputPressed,
			GetAbilitySpecHandle(),
			GetActivationPredictionKey()
		).Remove(DelegateHandle);
	}
	DelegateHandle.Reset();
}


