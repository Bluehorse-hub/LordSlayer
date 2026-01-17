// Shungen All Rights Reserved.


#include "AbilitySystem/Effect/GEExecCalc_DamageTaken.h"
#include "AbilitySystem/BluehorseAttributeSet.h"
#include "BluehorseGameplayTags.h"

#include "BluehorseDebugHelper.h"

// ExecutionCalculation で参照する Attribute をまとめて定義する構造体
struct FBluehorseDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)

	FBluehorseDamageCapture()
	{
		// 攻撃力：Source から取得
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBluehorseAttributeSet, AttackPower, Source, false)

		// 防御力：Target から取得
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBluehorseAttributeSet, DefensePower, Target, false)

		// 受けダメージ（メタ属性）：Target に書き込む先として使用
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBluehorseAttributeSet, DamageTaken, Target, false)
	}
};

// CaptureDef は毎回生成せず、static で 1 回だけ生成して使い回すのが定番
static const FBluehorseDamageCapture& GetBluehorseDamageCapture()
{
	static FBluehorseDamageCapture BluehorseDamageCapture;
	return BluehorseDamageCapture;
}


UGEExecCalc_DamageTaken::UGEExecCalc_DamageTaken()
{
	// この ExecCalc が参照する Attribute を登録する
	// ここに入れておかないと AttemptCalculateCapturedAttributeMagnitude で評価できない
	RelevantAttributesToCapture.Add(GetBluehorseDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetBluehorseDamageCapture().DefensePowerDef);
	RelevantAttributesToCapture.Add(GetBluehorseDamageCapture().DamageTakenDef);
}

//  ダメージ量を算出して DamageTaken（メタ属性）へ反映する
void UGEExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

	// Tag による条件分岐（バフ/デバフ/耐性等）を Capture 評価に反映するためのパラメータ
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	// ---- Source: AttackPower ------------------------------------------------
	float SourceAttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetBluehorseDamageCapture().AttackPowerDef, EvaluateParameters, SourceAttackPower);

	// ---- SetByCaller: ダメージパラメータ -------------------------------------
	// Ability 側から渡された値（BaseDamage / AttackType / MotionValue）を読む
	float BaseDamage = 0.f;
	float MotionValue = 1.0f;
	float AttackTypeMultiplier = 1.0f;

	// SetByCaller は Tag->Magnitude の辞書として渡されるため、該当タグを拾う
	for (const TPair<FGameplayTag, float>& TagMagnitude : EffectSpec.SetByCallerTagMagnitudes)
	{
		if (TagMagnitude.Key.MatchesTagExact(BluehorseGameplayTags::Shared_SetByCaller_BaseDamage))
		{
			BaseDamage = TagMagnitude.Value;
		}

		// 軽攻撃：モーション値を受け取り、タイプ倍率は 1.0
		if (TagMagnitude.Key.MatchesTagExact(BluehorseGameplayTags::Player_SetByCaller_AttackType_Light))
		{
			MotionValue = TagMagnitude.Value;
			AttackTypeMultiplier = 1.0f;
		}

		// 強攻撃：モーション値を受け取り、タイプ倍率は 1.5
		if (TagMagnitude.Key.MatchesTagExact(BluehorseGameplayTags::Player_SetByCaller_AttackType_Heavy))
		{
			MotionValue = TagMagnitude.Value;
			AttackTypeMultiplier = 1.5f;
		}
	}

	// ---- Target: DefensePower ----------------------------------------------
	float TargetDefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetBluehorseDamageCapture().DefensePowerDef, EvaluateParameters, TargetDefensePower);

	// ---- Final Damage -------------------------------------------------------
	// 最終ダメージ計算：
	// BaseDamage * (攻撃力/防御力) * 攻撃タイプ倍率 * モーション値
	const float FinalDamageDone = BaseDamage * SourceAttackPower / TargetDefensePower * AttackTypeMultiplier * MotionValue;

	// 0 以下なら反映しない
	if (FinalDamageDone > 0.f)
	{
		// DamageTaken は “今回のダメージ量” を入れるメタ属性なので Override で上書きする運用
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				GetBluehorseDamageCapture().DamageTakenProperty,
				EGameplayModOp::Override,
				FinalDamageDone
			)
		);
	}
}
