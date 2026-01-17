// Shungen All Rights Reserved.


#include "AbilitySystem/AbilityTasks/AbilityTask_ExecuteTaskOnTick.h"

UAbilityTask_ExecuteTaskOnTick::UAbilityTask_ExecuteTaskOnTick()
{
	// AbilityTask を Tick させるためのフラグ。
	// これを true にしないと TickTask が呼ばれない（Tick型タスクとして動作しない）
	bTickingTask = true;
}

// 毎フレーム処理を行うための Tick 型 AbilityTask を生成するファクトリ関数
UAbilityTask_ExecuteTaskOnTick* UAbilityTask_ExecuteTaskOnTick::ExecuteTaskOnTick(UGameplayAbility* OwningAbility)
{
	// AbilityTaskを生成する
	UAbilityTask_ExecuteTaskOnTick* Node = NewAbilityTask<UAbilityTask_ExecuteTaskOnTick>(OwningAbility);
	return Node;
}

// Task の Tick。
void UAbilityTask_ExecuteTaskOnTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	// AbilityTask の Delegate を呼んでも安全かどうか
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		// 毎フレーム DeltaTime を渡して呼び出し側で更新処理を行う
		OnAbilityTaskTick.Broadcast(DeltaTime);
	}
	else
	{
		// 不要な Tick 継続や破棄済み参照を避けるため、ここでタスクを終了する
		EndTask();
	}
}
