// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/BluehorseWidgetBase.h"
#include "BluehorseDoorWidget.generated.h"

/**
 * 
 */

class ABluehorseDoorInteract;

UCLASS()
class BLUEHORSE_API UBluehorseDoorWidget : public UBluehorseWidgetBase
{
	GENERATED_BODY()
	
public:
    // ドアが自分自身を渡して初期化する（所有先を固定）
    UFUNCTION(BlueprintCallable, Category = "Door")
    void InitDoor(ABluehorseDoorInteract* InDoor);

protected:

	FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
    TWeakObjectPtr<ABluehorseDoorInteract> OwnerDoor;
};
