// Shungen All Rights Reserved.


#include "AbilitySystem/BluehorseAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "BluehorseFunctionLibrary.h"
#include "BluehorseGameplayTags.h"
#include "Interfaces/PawnUIInterface.h"
#include "Components/UI/PawnUIComponent.h"
#include "Components/UI/HeroUIComponent.h"

#include "BluehorseDebugHelper.h"

UBluehorseAttributeSet::UBluehorseAttributeSet()
{
	// AttributeSet の初期値。
	// ※ここは「初期スポーン時に 0 になって UI が崩れる」などを避けるため、最低限の値を入れておく運用。
	InitCurrentHealth(1.f);
	InitMaxHealth(1.f);
	InitCurrentSkillPoint(1.f);
	InitMaxSkillPoint(1.f);
	InitCurrentUltimatePoint(1.f);
	InitMaxUltimatePoint(1.f);
	InitCurrentStamina(1.f);
	InitMaxStamina(1.f);
	InitAttackPower(1.f);
	InitDefensePower(1.f);
	
}

// GameplayEffect による Attribute 変更が「確定」した後に呼ばれるコールバック
void UBluehorseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// UI 通知先を毎回探索するとコストが増えるため、Interface を Weak 参照でキャッシュする
	if (!CachedPawnUIInterface.IsValid())
	{
		CachedPawnUIInterface = TWeakInterfacePtr<IPawnUIInterface>(Data.Target.GetAvatarActor());
	}

	checkf(CachedPawnUIInterface.IsValid(), TEXT("%s did not implement IPawnUIInterface"), *Data.Target.GetAvatarActor()->GetActorNameOrLabel());

	UPawnUIComponent* PawnUIComponent = CachedPawnUIInterface->GetPawnUIComponent();

	// UI コンポーネント取得に失敗した場合も、設計上は致命的なので即検知する
	checkf(PawnUIComponent, TEXT("Could not extrac a PawnUIComponent from %s"), *Data.Target.GetAvatarActor()->GetActorNameOrLabel());

	// Data.EvaluatedData.Attributeは今回変化したAttributeはどれかを判断する
	// ---- Health -------------------------------------------------------------
	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		// Current は 0~Max に収めて不正値を排除
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth());

		SetCurrentHealth(NewCurrentHealth);

		// UI は割合（0~1）で受け取る設計
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());
	}

	// ---- Skill Point --------------------------------------------------------
	if (Data.EvaluatedData.Attribute == GetCurrentSkillPointAttribute())
	{
		const float NewCurrentSkillPoint = FMath::Clamp(GetCurrentSkillPoint(), 0.f, GetMaxSkillPoint());

		SetCurrentSkillPoint(NewCurrentSkillPoint);

		// Hero 専用 UI が存在する場合のみ通知（敵など PawnUIComponent だけのケースも想定）
		if (UHeroUIComponent* HeroUIComponent = CachedPawnUIInterface->GetHeroUIComponent())
		{
			HeroUIComponent->OnCurrentSkillPointChanged.Broadcast(GetCurrentSkillPoint() / GetMaxSkillPoint());
		}
	}

	// ---- Ultimate Point -----------------------------------------------------
	if (Data.EvaluatedData.Attribute == GetCurrentUltimatePointAttribute())
	{
		const float NewCurrentUltimatePoint = FMath::Clamp(GetCurrentUltimatePoint(), 0.f, GetMaxUltimatePoint());

		SetCurrentUltimatePoint(NewCurrentUltimatePoint);

		if (UHeroUIComponent* HeroUIComponent = CachedPawnUIInterface->GetHeroUIComponent())
		{
			// UI 側の ProgressBar が「TopToBottom」で、かつ「溜める系」の表現をしているため、
			// 表示上は 1 - (Current/Max) で埋まり方を反転させる。
			HeroUIComponent->OnCurrentUltimatePointChanged.Broadcast(1.f - (GetCurrentUltimatePoint() / GetMaxUltimatePoint()));
		}
	}

	// ---- Stamina ------------------------------------------------------------
	if (Data.EvaluatedData.Attribute == GetCurrentStaminaAttribute())
	{
		const float NewCurrentStamina = FMath::Clamp(GetCurrentStamina(), 0.f, GetMaxStamina());

		SetCurrentStamina(NewCurrentStamina);

		if (UHeroUIComponent* HeroUIComponent = CachedPawnUIInterface->GetHeroUIComponent())
		{
			HeroUIComponent->OnCurrentStaminaChanged.Broadcast(GetCurrentStamina() / GetMaxStamina());
		}
	}

	// ---- Damage Taken (meta attribute) -------------------------------------
	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		// DamageTaken は「受けたダメージ量」を一時的に受け取るメタ属性として運用し、
		// ここで Health へ反映する。
		const float OldHealth = GetCurrentHealth();
		const float DamageDone = GetDamageTaken();

		const float NewCurrentHealth = FMath::Clamp(OldHealth - DamageDone, 0.f, GetMaxHealth());

		SetCurrentHealth(NewCurrentHealth);

		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());
	}

	// ---- Death --------------------------------------------------------------
	// Health が 0 になったら死亡タグを付与する
	if (GetCurrentHealth() == 0.f)
	{
		UBluehorseFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), BluehorseGameplayTags::Shared_Status_Dead);
	}
}
