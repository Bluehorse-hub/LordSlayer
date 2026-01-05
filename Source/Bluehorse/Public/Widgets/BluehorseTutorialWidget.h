// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/BluehorseWidgetBase.h"
#include "BluehorseTutorialWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorseTutorialWidget : public UBluehorseWidgetBase
{
	GENERATED_BODY()

protected:
	FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION(BlueprintCallable)
	void HideOtherWidgets();

	UFUNCTION(BlueprintCallable)
	void RestoreOtherWidgets();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* RemoveSound;

	UPROPERTY(Transient)
	TMap<TObjectPtr<UUserWidget>, ESlateVisibility> CachedWidgetVisibility;
	
};
