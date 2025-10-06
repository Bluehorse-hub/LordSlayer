// Shungen All Rights Reserved.


#include "Components/UI/EnemyUIComponent.h"
#include "Widgets/BluehorseWidgetBase.h"

void UEnemyUIComponent::RegisterEnemyDrawnWidget(UBluehorseWidgetBase* InWidgetToRegister)
{
	EnemyDrawnWidgets.Add(InWidgetToRegister);
}

void UEnemyUIComponent::RemoveEnemyDrawnEnemyWidgetsIfAny()
{
	if (EnemyDrawnWidgets.IsEmpty())
	{
		return;
	}

	for (UBluehorseWidgetBase* DrawnWidget : EnemyDrawnWidgets)
	{
		if (DrawnWidget)
		{
			DrawnWidget->RemoveFromParent();
		}
	}

	EnemyDrawnWidgets.Empty();
}
