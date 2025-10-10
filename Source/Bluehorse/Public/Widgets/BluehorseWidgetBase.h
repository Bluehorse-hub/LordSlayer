// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BluehorseWidgetBase.generated.h"

class UHeroUIComponent;
class UEnemyUIComponent;

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UBluehorseWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// --- ���͉��̎�� ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* MoveSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* DecideSound;

	virtual void NativeOnInitialized() override;

	// ������BP���ŏ������`����̂�cpp�t�@�C�����ɏ����͏����Ȃ�
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Owning Hero UI Component Initialized"))
	void BP_OnOwningHeroUIComponentInitialized(UHeroUIComponent* OnOwningHeroUIComponent);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Owning Enemy UI Component Initialized"))
	void BP_OnOwningEnemyUIComponentInitialized(UEnemyUIComponent* OnOwningEnemyUIComponent);

	void PlayUISound(USoundBase* SoundToPlay);

public:
	UFUNCTION(BlueprintCallable)
	void InitEnemyCreatedWidget(AActor* OwningEnemyActor);
};
