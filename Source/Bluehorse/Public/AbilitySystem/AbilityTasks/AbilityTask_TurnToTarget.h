// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayTagContainer.h" 
#include "AbilityTask_TurnToTarget.generated.h"

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UAbilityTask_TurnToTarget : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAbilityTask_TurnToTarget();

	UFUNCTION(BlueprintCallable, Category = "Bluehorse|AbilityTasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_TurnToTarget* TurnToTarget(UGameplayAbility* OwningAbility, AActor* TargetActor, float InterpSpeed, FGameplayTag EndTag);

	
	
protected:
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	/** �ǔ��ΏۂƂȂ�A�N�^�[ */
	UPROPERTY()
	TWeakObjectPtr<AActor> Target;

	/** ��]��ԑ��x */
	UPROPERTY()
	float RotationSpeed = 10.f;

	UPROPERTY()
	FGameplayTag EndCheckTag;
};
