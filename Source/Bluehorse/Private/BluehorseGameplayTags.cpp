// Shungen All Rights Reserved.


#include "BluehorseGameplayTags.h"

namespace BluehorseGameplayTags
{
	//---Input Tags ---//
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Roll, "InputTag.Roll");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Block, "InputTag.Block");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Parry, "InputTag.Parry");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Toggleable, "InputTag.Toggleable");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Toggleable_TargetLock, "InputTag.Toggleable.TargetLock");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Equip_Sword, "InputTag.Equip.Sword");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Unequip_Sword, "InputTag.Unequip.Sword");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_LightAttack_Sword, "InputTag.LightAttack.Sword");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_HeavyAttack_Sword, "InputTag.HeavyAttack.Sword");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Skill, "InputTag.Skill");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Interact, "InputTag.Interact");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_UsePotion, "InputTag.UsePotion");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_UseItem, "InputTag.UseItem");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_SelectItem, "InputTag.SelectItem");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_SwitchTarget, "InputTag.SwitchTarget");

	//--- Item Tags ---//
	UE_DEFINE_GAMEPLAY_TAG(Item_Potion, "Item.Potion");
	UE_DEFINE_GAMEPLAY_TAG(Item_Aether, "Item.Aether");

	//--- Player Tags ---//
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Move, "Player.Ability.Move");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Look, "Player.Ability.Look");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Equip_Sword, "Player.Ability.Equip.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Unequip_Sword, "Player.Ability.Unequip.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Interact, "Player.Ability.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_UsePotion, "Player.Ability.UsePotion");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_UseItem, "Player.Ability.UseItem");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_SelectItem, "Player.Ability.SelectItem");

	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack, "Player.Ability.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Sword_Light, "Player.Ability.Attack.Sword.Light");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Sword_Light_1, "Player.Ability.Attack.Sword.Light.1");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Sword_Light_2, "Player.Ability.Attack.Sword.Light.2");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Sword_Light_3, "Player.Ability.Attack.Sword.Light.3");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Sword_Light_4, "Player.Ability.Attack.Sword.Light.4");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Sword_Heavy, "Player.Ability.Attack.Sword.Heavy");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Sword_Heavy_1, "Player.Ability.Attack.Sword.Heavy.1");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Sword_Heavy_2, "Player.Ability.Attack.Sword.Heavy.2");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Sword_Heavy_3, "Player.Ability.Attack.Sword.Heavy.3");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Sword_Heavy_4, "Player.Ability.Attack.Sword.Heavy.4");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_HitPause, "Player.Ability.HitPause");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Roll_Combat, "Player.Ability.Roll.Combat");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Roll_NotCombat, "Player.Ability.Roll.NotCombat");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Block, "Player.Ability.Block");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_TargetLock, "Player.Ability.TargetLock");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Parry, "Player.Ability.Parry");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_MustBeHeld, "Player.Ability.MustBeHeld");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Slash, "Player.Ability.Slash");

	UE_DEFINE_GAMEPLAY_TAG(Player_Weapon_Sword, "Player.Weapon.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Player_Weapon_Shield, "Player.Weapon.Shield");

	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Equip_Sword, "Player.Event.Equip.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Unequip_Sword, "Player.Event.Unequip.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Equip_Shield, "Player.Event.Equip.Shield");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Unequip_Shield, "Player.Event.Unequip.Shield");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_HitPause, "Player.Event.HitPause");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_SuccessfulBlock, "Player.Event.SuccessfulBlock");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_SuccessfulParry, "Player.Event.SuccessfulParry");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Combo_PreInput, "Player.Event.Combo.PreInput");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Combo_NextAttack, "Player.Event.Combo.NextAttack");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_SwitchTarget_Left, "Player.Event.SwitchTarget.Left");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_SwitchTarget_Right, "Player.Event.SwitchTarget.Right");

	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Combat, "Player.Status.Combat");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Rolling, "Player.Status.Rolling");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Blocking, "Player.Status.Blocking");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_TargetLock, "Player.Status.TargetLock");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Parrying, "Player.Status.Parrying");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Invincible, "Player.Status.Invincible");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_NoCancel, "Player.Status.NoCancel");

	UE_DEFINE_GAMEPLAY_TAG(Player_SetByCaller_AttackType_Light, "Player.SetByCaller.AttackType.Light");
	UE_DEFINE_GAMEPLAY_TAG(Player_SetByCaller_AttackType_Heavy, "Player.SetByCaller.AttackType.Heavy");

	//--- Enemy Tags ---//
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Type_Minion, "Enemy.Type.Minion");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Type_Boss, "Enemy.Type.Boss");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Melee, "Enemy.Ability.Melee");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Ranged, "Enemy.Ability.Ranged");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Weapon_Sword, "Enemy.Weapon.Sword");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_Strafing, "Enemy.Status.Strafing");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_UnderAttack, "Enemy.Status.UnderAttack");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Event_Knockdown, "Enemy.Event.Knockdown");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Event_TurnToTarget_Begin, "Enemy.Event.TurnToTarget.Begin");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Event_TurnToTarget_End, "Enemy.Event.TurnToTarget.End");

	//--- Shared Tags ---//
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_HitReact, "Shared.Ability.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_Knockdown, "Shared.Ability.Knockdown");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_Death, "Shared.Ability.Death");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_Executed, "Shared.Ability.Executed");

	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_HitReact, "Shared.Event.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_MeleeHit, "Shared.Event.MeleeHit");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_Executed, "Shared.Event.Executed");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_SpawnProjectile, "Shared.Event.SpawnProjectile");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_SpawnAOE, "Shared.Event.SpawnAOE");

	UE_DEFINE_GAMEPLAY_TAG(Shared_SetByCaller_BaseDamage, "Shared.SetByCaller.BaseDamage");

	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_Dead, "Shared.Status.Dead");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Front, "Shared.Status.HitReact.Front");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Back, "Shared.Status.HitReact.Back");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Right, "Shared.Status.HitReact.Right");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Left, "Shared.Status.HitReact.Left");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_Knockdown, "Shared.Status.Knockdown");

	//--- Game Data Tags ---//
	UE_DEFINE_GAMEPLAY_TAG(GameData_Level_WaitingAreaMap, "GameData.Level.WaitingAreaMap");
	UE_DEFINE_GAMEPLAY_TAG(GameData_Level_CombatGameModeMap, "GameData.Level.CombatGameModeMap");
	UE_DEFINE_GAMEPLAY_TAG(GameData_Level_BossMap, "GameData.Level.BossMap");
	UE_DEFINE_GAMEPLAY_TAG(GameData_Level_TitleMap, "GameData.Level.TitleMap");
}
