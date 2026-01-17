// Shungen All Rights Reserved.
//--- 現在未使用 ---// 

#include "AbilityTask/AbilityTask_WaitAcceleration.h"
#include "Abilities/GameplayAbility.h"

UAbilityTask_WaitAcceleration* UAbilityTask_WaitAcceleration::WaitAcceleration(
    UGameplayAbility* OwningAbility, float InDeadZone, float InSampleInterval, bool InBroadcastInitial)
{
    UAbilityTask_WaitAcceleration* Task = NewAbilityTask<UAbilityTask_WaitAcceleration>(OwningAbility);
    Task->DeadZoneSq = FMath::Square(InDeadZone);
    Task->SampleInterval = FMath::Max(0.f, InSampleInterval); // 0なら毎フレ
    Task->bBroadcastInitial = InBroadcastInitial;
    return Task;
}

void UAbilityTask_WaitAcceleration::Activate()
{
    bTickingTask = true;

    AActor* Avatar = GetAvatarActor();
    if (ACharacter* C = Cast<ACharacter>(Avatar))
    {
        Move = C->GetCharacterMovement();
    }

    bool Now = false;
    if (SampleHasAccel(Now))
    {
        bLast = Now;
        if (bBroadcastInitial)
        {
            OnChanged.Broadcast(bLast);
            if (bLast) OnBegin.Broadcast();
        }
    }
    else
    {
        EndTask(); // 取れないなら終了
    }
}

bool UAbilityTask_WaitAcceleration::SampleHasAccel(bool& bOutHasAccel) const
{
    if (!Move.IsValid()) return false;
    const FVector Acc = Move->GetCurrentAcceleration();
    bOutHasAccel = (Acc.SizeSquared2D() > DeadZoneSq);
    return true;
}

void UAbilityTask_WaitAcceleration::TickTask(float DeltaTime)
{
    Super::TickTask(DeltaTime);
    if (!Move.IsValid())
    {
        EndTask();
        return;
    }

    TimeAcc += DeltaTime;
    if (SampleInterval > 0.f && TimeAcc < SampleInterval) return;
    TimeAcc = 0.f;

    bool Now = false;
    if (!SampleHasAccel(Now)) { EndTask(); return; }

    if (Now != bLast)
    {
        bLast = Now;
        OnChanged.Broadcast(bLast);
        (bLast ? OnBegin : OnEnd).Broadcast();
    }
}

void UAbilityTask_WaitAcceleration::OnDestroy(bool bInOwnerFinished)
{
    bTickingTask = false;
    Move.Reset();
    Super::OnDestroy(bInOwnerFinished);
}