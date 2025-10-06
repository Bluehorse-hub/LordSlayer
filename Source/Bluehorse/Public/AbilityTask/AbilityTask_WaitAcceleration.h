// Shungen All Rights Reserved.

// 現在未使用

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilityTask_WaitAcceleration.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAccelChanged, bool, bHasAcceleration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAccelEvent); // Begin/End

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UAbilityTask_WaitAcceleration : public UAbilityTask
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable) FAccelChanged OnChanged;
    UPROPERTY(BlueprintAssignable) FAccelEvent   OnBegin;   // 0→1
    UPROPERTY(BlueprintAssignable) FAccelEvent   OnEnd;     // 1→0

    // 使い方：UAT_WaitAcceleration::WaitAcceleration(this, 0.1f);
    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
        meta = (DisplayName = "Wait Acceleration Change", DefaultToSelf = "OwningAbility",
            BlueprintInternalUseOnly = "true"))
    static UAbilityTask_WaitAcceleration* WaitAcceleration(UGameplayAbility* OwningAbility,
        float DeadZone = 0.1f,
        float SampleInterval = 0.f,
        bool bBroadcastInitial = true);

    virtual void Activate() override;
    virtual void TickTask(float DeltaTime) override;
    virtual void OnDestroy(bool bInOwnerFinished) override;

private:
    TWeakObjectPtr<UCharacterMovementComponent> Move;
    float DeadZoneSq = 0.f;
    float SampleInterval = 0.f;
    bool  bBroadcastInitial = true;

    bool  bLast = false;
    float TimeAcc = 0.f;

    bool SampleHasAccel(bool& bOutHasAccel) const;
	
};
