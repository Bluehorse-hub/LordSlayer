// Shungen All Rights Reserved.


#include "Components/UI/EnemyUIComponent.h"
#include "Widgets/BluehorseWidgetBase.h"

// 敵キャラクターに表示されている Widget を登録する
void UEnemyUIComponent::RegisterEnemyDrawnWidget(UBluehorseWidgetBase* InWidgetToRegister)
{
	EnemyDrawnWidgets.Add(InWidgetToRegister);
}

// 現在登録されている Enemy 用 Widget をすべて画面から削除する
void UEnemyUIComponent::RemoveEnemyDrawnEnemyWidgetsIfAny()
{
	// 登録されている Widget が無ければ何もしない
	if (EnemyDrawnWidgets.IsEmpty())
	{
		return;
	}

	// すべての Widget を親から削除
	for (UBluehorseWidgetBase* DrawnWidget : EnemyDrawnWidgets)
	{
		if (DrawnWidget)
		{
			DrawnWidget->RemoveFromParent();
		}
	}

	// 管理リストをクリア
	EnemyDrawnWidgets.Empty();
}
