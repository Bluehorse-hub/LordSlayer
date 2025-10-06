// Shungen All Rights Reserved.


#include "AbilitySystem/Effect/GEExecCalc_DamageTaken.h"
#include "AbilitySystem/BluehorseAttributeSet.h"
#include "BluehorseGameplayTags.h"

#include "BluehorseDebugHelper.h"

struct FBluehorseDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)

	FBluehorseDamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBluehorseAttributeSet, AttackPower, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBluehorseAttributeSet, DefensePower, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBluehorseAttributeSet, DamageTaken, Target, false)
	}
};

static const FBluehorseDamageCapture& GetBluehorseDamageCapture()
{
	static FBluehorseDamageCapture BluehorseDamageCapture;
	return BluehorseDamageCapture;
}


UGEExecCalc_DamageTaken::UGEExecCalc_DamageTaken()
{
	RelevantAttributesToCapture.Add(GetBluehorseDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetBluehorseDamageCapture().DefensePowerDef);
	RelevantAttributesToCapture.Add(GetBluehorseDamageCapture().DamageTakenDef);
}

void UGEExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	float SourceAttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetBluehorseDamageCapture().AttackPowerDef, EvaluateParameters, SourceAttackPower);

	float BaseDamage = 0.f;

	for (const TPair<FGameplayTag, float>& TagMagnitude : EffectSpec.SetByCallerTagMagnitudes)
	{
		if (TagMagnitude.Key.MatchesTagExact(BluehorseGameplayTags::Shared_SetByCaller_BaseDamage))
		{
			BaseDamage = TagMagnitude.Value;
			// Debug::Print(TEXT("BaseDamage"), BaseDamage);
		}
		if (TagMagnitude.Key.MatchesTagExact(BluehorseGameplayTags::Player_SetByCaller_AttackType_Light))
		{
			// TODO: LightAttack‚Ì”{—¦ŒvŽZ‚ðl‚¦‚é
			// Debug::Print(TEXT("Light Attack Done"));
		}

		if (TagMagnitude.Key.MatchesTagExact(BluehorseGameplayTags::Player_SetByCaller_AttackType_Heavy))
		{
			// TODO: HeavyAttack‚Ì”{—¦ŒvŽZ‚ðl‚¦‚é
			// Debug::Print(TEXT("Heavy Attack Done"));
		}
	}

	float TargetDefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetBluehorseDamageCapture().DefensePowerDef, EvaluateParameters, TargetDefensePower);

	const float FinalDamageDone = BaseDamage * SourceAttackPower / TargetDefensePower;
	// Debug::Print(TEXT("FinalDamageDone"), FinalDamageDone);

	if (FinalDamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				GetBluehorseDamageCapture().DamageTakenProperty,
				EGameplayModOp::Override,
				FinalDamageDone
			)
		);
	}
}
