// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/BluehorseWidgetBase.h"
#include "BluehorseMainMenuScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorseMainMenuScreenWidget : public UBluehorseWidgetBase
{
	GENERATED_BODY()

protected:

	FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "TitleScreenWidget")
	void BP_OnFaceButtonBottom();

	UFUNCTION(BlueprintImplementableEvent, Category = "TitleScreenWidget")
	void BP_OnHighlightChanged(int32 Index, bool bIsHighlighted);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int NumButton;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int CurrentIndex = 0;
};
