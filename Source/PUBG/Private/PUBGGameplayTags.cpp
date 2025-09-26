// Fill out your copyright notice in the Description page of Project Settings.


#include "PUBGGameplayTags.h"

namespace PUBGGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_AbilityInputBlocked, "Gameplay.AbilityInputBlocked");

	// 입력 태그
	UE_DEFINE_GAMEPLAY_TAG(Input_Move, "Input.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Turn, "Input.Turn");
	UE_DEFINE_GAMEPLAY_TAG(Input_Jump, "Input.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Input_FreeView, "Input.FreeView");

	UE_DEFINE_GAMEPLAY_TAG(Input_Crouch, "Input.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Input_Prone, "Input.Prone");
	
	UE_DEFINE_GAMEPLAY_TAG(Input_Walk, "Input.Walk");
	UE_DEFINE_GAMEPLAY_TAG(Input_Sprint, "Input.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Input_Interact, "Input.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Input_FPS, "Input.FPS");
	
	UE_DEFINE_GAMEPLAY_TAG(Input_Active_EquipmentMain, "Input.Active.EquipmentMain");
	UE_DEFINE_GAMEPLAY_TAG(Input_Active_EquipmentMain2, "Input.Active.EquipmentMain2");
	UE_DEFINE_GAMEPLAY_TAG(Input_Active_EquipmentSub, "Input.Active.EquipmentSub");
	UE_DEFINE_GAMEPLAY_TAG(Input_Active_EquipmentEtc, "Input.Active.EquipmentEtc");
	UE_DEFINE_GAMEPLAY_TAG(Input_Active_EquipmentThrow, "Input.Active.EquipmentThrow");
	UE_DEFINE_GAMEPLAY_TAG(Input_Active_UnequipmentWeapon, "Input.Active.UnequipmentWeapon");
	
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Primary, "Input.Action.Primary");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Secondary, "Input.Action.Secondary");
	
	// 상태
	UE_DEFINE_GAMEPLAY_TAG(Status_StateTransform, "Status.StateTransform");
	UE_DEFINE_GAMEPLAY_TAG(Status_Stand, "Status.Stand");
	UE_DEFINE_GAMEPLAY_TAG(Status_Crouch, "Status.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Status_Prone, "Status.Prone");
	UE_DEFINE_GAMEPLAY_TAG(Status_Jump, "Status.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Status_Sprint, "Status.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Status_Walk, "Status.Walk");
	UE_DEFINE_GAMEPLAY_TAG(Status_Death, "Status.Death");
	UE_DEFINE_GAMEPLAY_TAG(Status_UseEquipItem, "Status.UseEquipItem");
	UE_DEFINE_GAMEPLAY_TAG(Status_AimOffset_MainGun, "Status.AimOffset.MainGun");
	UE_DEFINE_GAMEPLAY_TAG(Status_AimOffset_SubGun, "Status.AimOffset.SubGun");
	UE_DEFINE_GAMEPLAY_TAG(Status_AimOffset_Throw, "Status.AimOffset.Throw");
	UE_DEFINE_GAMEPLAY_TAG(Status_Aiming, "Status.Aiming");
	UE_DEFINE_GAMEPLAY_TAG(Status_Aim_Scope, "Status.Aim.Scope");
	UE_DEFINE_GAMEPLAY_TAG(Status_Aim_ADS, "Status.Aim.ADS");
	UE_DEFINE_GAMEPLAY_TAG(Status_FreeFalling, "Status.FreeFalling");
	UE_DEFINE_GAMEPLAY_TAG(Status_UmbrellaOpened, "Status.UmbrellaOpened");
	UE_DEFINE_GAMEPLAY_TAG(Status_Drink, "Status.Drink");
	UE_DEFINE_GAMEPLAY_TAG(Status_Shot, "Status.Shot");
	UE_DEFINE_GAMEPLAY_TAG(Status_Interact, "Status.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Status_Weapon_Switching, "Status.Weapon.Switching");
	UE_DEFINE_GAMEPLAY_TAG(Status_Attack, "Status.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Status_FPS, "Status.FPS");
	//UE_DEFINE_GAMEPLAY_TAG(Status_HandItem_AK47, "Status.HandItem.AK47");

	// Ability
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_ActivationGroup, "Ability.ActivateFail.ActivationGroup");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_IsDead, "Ability.ActivateFail.IsDead");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Behavior_SurvivesDeath, "Ability.Behavior.SurvivesDeath");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_StateTransform, "Ability.StateTransform");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Stand, "Ability.Stand");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Crouch, "Ability.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Prone, "Ability.Prone");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Jump, "Ability.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Sprint_Check, "Ability.Sprint.Check");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Sprint_Active, "Ability.Sprint.Active");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Walk, "Ability.Walk");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Death, "Ability.Death");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Block_Move, "Ability.Block.Move");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Block_AbilityGameOnly, "Ability.Block.AbilityGameOnly");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Interact_Active, "Ability.Interact.Active");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Interact_Object, "Ability.Interact.Object");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActiveEquipment, "Ability.ActiveEquipment");
	UE_DEFINE_GAMEPLAY_TAG(Ability_UnActiveEquipment, "Ability.UnActiveEquipment");
	UE_DEFINE_GAMEPLAY_TAG(Ability_SwapActiveEquipment, "Ability.SwapActiveEquipment");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_1, "Ability.Attack.1");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_2, "Ability.Attack.2");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_3, "Ability.Attack.3");
	UE_DEFINE_GAMEPLAY_TAG(Ability_AimDownSightMode, "Ability.AimDownSightMode");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ScopeMode, "Ability.ScopeMode");
	UE_DEFINE_GAMEPLAY_TAG(Ability_FPS, "Ability.FPS");

	// Weapon
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Suppressor, "Weapon.Suppressor");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_NotFlash, "Weapon.NotFlash");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_1xScope, "Weapon.1xScope");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_4xScope, "Weapon.4xScope");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_8xScope, "Weapon.8xScope");

	// Gameplay Event
	// UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Death, "GameplayEvent.Death");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_StateTransform_Timer, "GameplayEvent.StateTransform.Timer");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_StateTransform_Start, "GameplayEvent.StateTransform.Start");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_StateTransform_End, "GameplayEvent.StateTransform.End");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Block_Move, "GameplayEvent.Block.Move");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Block_AbilityGameOnly, "GameplayEvent.Block.AbilityGameOnly");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Interact, "GameplayEvent.Interact");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Stand, "GameplayEvent.Stand");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_ActiveEquipment_Swap, "GameplayEvent.ActiveEquipment.Swap");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Input_Pressed, "GameplayEvent.Input.Pressed");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Trace, "GameplayEvent.Trace");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Death, "GameplayEvent.Death");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Montage_Begin, "GameplayEvent.Montage.Begin");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Montage_Tick, "GameplayEvent.Montage.Tick");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Montage_End, "GameplayEvent.Montage.End");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Montage_Play, "GameplayEvent.Montage.Play");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_AimDownSightMode, "GameplayEvent.AimDownSightMode");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_ScopeMode, "GameplayEvent.ScopeMode");

	// Cheat
	UE_DEFINE_GAMEPLAY_TAG(Cheat_GodMode, "Cheat.GodMode");
	UE_DEFINE_GAMEPLAY_TAG(Cheat_UnlimitedHealth, "Cheat.UnlimitedHealth");
	
	// Movement

	// Message
	UE_DEFINE_GAMEPLAY_TAG(Message_Interaction_Notice, "Message.Interaction.Notice");
	UE_DEFINE_GAMEPLAY_TAG(Message_Interaction_Progress, "Message.Interaction.Progress");
	UE_DEFINE_GAMEPLAY_TAG(Message_Interaction_Detect, "Message.Interaction.Detect");
	UE_DEFINE_GAMEPLAY_TAG(Message_AdditionalValue, "Message.AdditionalValue");

	// SetByCaller
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_BaseDamage, "SetByCaller.BaseDamage");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_BaseHealth, "SetByCaller.BaseHealth");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_IncomingDamage, "SetByCaller.IncomingDamage");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_IncomingHealth, "SetByCaller.IncomingHealth");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_IncomingERHealth, "SetByCaller.IncomingERHealth");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_IncomingStamina, "SetByCaller.IncomingStamina");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_MoveSpeed, "SetByCaller.MoveSpeed");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_MoveSpeedPercent, "SetByCaller.MoveSpeedPercent");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_AttackSpeedPercent, "SetByCaller.AttackSpeedPercent");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_HeadResistancePercent, "SetByCaller.HeadResistancePercent");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_BodyResistancePercent, "SetByCaller.BodyResistancePercent");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_EWeaponClassType, "SetByCaller.EWeaponClassType");
	
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_AdditionalMaxCapacityWeight, "SetByCaller.AdditionalMaxCapacityWeight");
	
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_ExtraClipSize, "SetByCaller.ExtraClipSize");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_ReplaceClipTimePercent, "SetByCaller.ReplaceClipTimePercent");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_VerticalRecoilPercent, "SetByCaller.VerticalRecoilPercent");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_HorizontalRecoilPercent, "SetByCaller.HorizontalRecoilPercent");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_AimingtimePercent, "SetByCaller.AimingtimePercent");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_HipfireAccuracyPercent, "SetByCaller.HipfireAccuracyPercent");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_AimedShootingAccuracyPercent, "SetByCaller.AimedShootingAccuracyPercent");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_FirstShotRecoilPercent, "SetByCaller.FirstShotRecoilPercent");

	// Variable
	UE_DEFINE_GAMEPLAY_TAG(Variable_Item_CurrentDurability, "Variable.Item.CurrentDurability");
	UE_DEFINE_GAMEPLAY_TAG(Variable_Item_MaxinumDurability, "Variable.Item.MaxinumDurability");
	
	// GameplayCue
	// UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Attack, "GameplayCue.Weapon.Attack");

	// Cooldown
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Status_Crouch, "Cooldown.Status.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Status_Prone, "Cooldown.Status.Prone");

	// HUD

	// Equip
	UE_DEFINE_GAMEPLAY_TAG(Equip_Helmet, "Equip.Helmet");
	
	UE_DEFINE_GAMEPLAY_TAG(Equip_Vest, "Equip.Vest");
	
	UE_DEFINE_GAMEPLAY_TAG(Equip_Backpack, "Equip.Backpack");
}