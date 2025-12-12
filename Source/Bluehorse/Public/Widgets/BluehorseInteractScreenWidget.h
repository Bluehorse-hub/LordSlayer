// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/BluehorseWidgetBase.h"
#include "BluehorseInteractScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorseInteractScreenWidget : public UBluehorseWidgetBase
{
	GENERATED_BODY()
	
protected:

	FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* InteractSound;

	UFUNCTION(BlueprintImplementableEvent, Category = "PortalWidget")
	void BP_OnFaceButtonTop();
};
