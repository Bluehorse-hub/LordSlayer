// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayTagContainer.h"
#include "AbilityTask_WaitComboBuffer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLocalInputPressed);

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UAbilityTask_WaitComboBuffer : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAbilityTask_WaitComboBuffer(const FObjectInitializer& ObjectInitializer);

	// bTestAlreadyPressed: Activate����Ɋ��ɉ�����ԂȂ瑦������
	// bTriggerOnce: true�Ȃ��x�ŏI���Afalse�Ȃ疾���I�� EndTask �܂ŌJ��Ԃ��󂯎��
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "Confirm Input In GA", DefaultToSelf = "OwningAbility", HidePin = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_WaitComboBuffer* ConfirmInput(UGameplayAbility* OwningAbility, bool bTriggerOnce = true);

	// �������ɒʒm
	UPROPERTY(BlueprintAssignable)
	FLocalInputPressed OnPressed;

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	UFUNCTION()
	void HandlePressed();
	void Bind();
	void Unbind();

private:
	bool bTestInitialState = true;
	bool bTriggerOnce = true;
	FDelegateHandle DelegateHandle;
};
