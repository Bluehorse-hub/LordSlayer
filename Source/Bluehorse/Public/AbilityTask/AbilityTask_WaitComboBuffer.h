// Shungen All Rights Reserved.

//--- 現在未使用 ---// 

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

	// bTestAlreadyPressed: Activate直後に既に押下状態なら即時発火
	// bTriggerOnce: trueなら一度で終了、falseなら明示的に EndTask まで繰り返し受け取る
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "Confirm Input In GA", DefaultToSelf = "OwningAbility", HidePin = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_WaitComboBuffer* ConfirmInput(UGameplayAbility* OwningAbility, bool bTriggerOnce = true);

	// 押下時に通知
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
