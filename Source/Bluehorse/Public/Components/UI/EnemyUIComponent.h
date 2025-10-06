// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/PawnUIComponent.h"
#include "EnemyUIComponent.generated.h"

class UBluehorseWidgetBase;

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UEnemyUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void RegisterEnemyDrawnWidget(UBluehorseWidgetBase* InWidgetToRegister);

	UFUNCTION(BlueprintCallable)
	void RemoveEnemyDrawnEnemyWidgetsIfAny();
	
private:
	TArray<UBluehorseWidgetBase*> EnemyDrawnWidgets;
};
