#include "RefrainGameplayTags.h"

namespace RefrainGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack, "Ability.Attack", "기본 공격");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Attack, "GameplayCue.Attack", "공격 이펙트");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_ComboStart, "Event.Montage.Combo.Start", "콤보 시작");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_ComboEnd, "Event.Montage.Combo.End", "콤보 완료");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Dodge, "Ability.Dodge", "회피");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_AttackHit, "Event.Montage.AttackHit", "공격 적중");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_Damage, "Data.Damage", "데미지");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Untargetable, "State.Untargetable", "대상 지정 불가");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_HitReact, "State.HitReact", "피격 중인 상태");
}
