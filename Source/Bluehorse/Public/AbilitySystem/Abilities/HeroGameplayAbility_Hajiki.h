// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BluehorseHeroGameplayAbility.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h" 
#include "HeroGameplayAbility_Hajiki.generated.h"

/**
 * 
 */
UCLASS()
class BLUEHORSE_API UHeroGameplayAbility_Hajiki : public UBluehorseHeroGameplayAbility
{
	GENERATED_BODY()

public:
	UHeroGameplayAbility_Hajiki();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
private:
	// ----- アニメーション制御 -----
	UFUNCTION(BlueprintCallable, Category = "Hajiki")
	int32 GetCurrentHajikiAnimIndexFromAttribute();

	// ----- エフェクト位置取得 -----
	UFUNCTION(BlueprintCallable, Category = "Hajiki")
	FVector GetHajikiEffectLocation(FName InSocketName);

	// ----- エフェクト生成 -----
	UFUNCTION(BlueprintCallable, Category = "Hajiki")
	void SpawnHajikiEffectAtLocation();

	UFUNCTION(BlueprintCallable, Category = "Hajiki")
	void UpdateHajikiCount();

	UFUNCTION()
	void OnHajikiMontageCompleted();

	UFUNCTION()
	void OnHajikiMontageInterrupted();

	UFUNCTION()
	void OnHajikiMontageCancelled();

	// 弾きアニメーション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hajiki|Animation", meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> HajikiMontages;

	// エフェクトを出すソケット名
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hajiki|Effect", meta = (AllowPrivateAccess = "true"))
	FName SocketName;

	// Niagaraエフェクト本体
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hajiki|Effect", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* HajikiEffect;

	// 再生するサウンド
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hajiki|Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* HajikiSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hajiki", meta = (AllowPrivateAccess = "true"))
	int32 CountForReinforcement = 10;
};
