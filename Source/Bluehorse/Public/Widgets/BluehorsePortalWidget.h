// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/BluehorseWidgetBase.h"
#include "BluehorsePortalWidget.generated.h"

class UBluehorseWidgetBase;
/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorsePortalWidget : public UBluehorseWidgetBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* InteractSound;

	FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "PortalWidget")
	void BP_OnFaceButtonBottom();

	UFUNCTION(BlueprintImplementableEvent, Category = "PortalWidget")
	void BP_OnFaceButtonTop();

	UFUNCTION(BlueprintImplementableEvent, Category = "PortalWidget")
	void BP_OnHighlightChanged(int32 Index, bool bIsHighlighted);

private:
	UPROPERTY(EditDefaultsOnly ,BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int NumButton;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int CurrentIndex = 0;

	bool IsSelectWidget = false;
};
