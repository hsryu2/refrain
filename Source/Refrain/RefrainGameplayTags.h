#pragma once

#include "NativeGameplayTags.h"

namespace RefrainGameplayTags
{
	// Attack 태그.
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack);
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Attack);
	
	// AttackCombo 시작, 완료 태그
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_ComboStart);
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_ComboEnd);
	
	// Dodge 태그.
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dodge);
	
	// 데미지 관련 태그
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_AttackHit);
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage);
	
	// 캐릭터 상태
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Untargetable);
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_HitReact);
}
