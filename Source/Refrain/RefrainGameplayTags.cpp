#include "RefrainGameplayTags.h"

namespace RefrainGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Test_GameplayTag_Example, "Test.GameplayTag.Example", "코멘트입니다");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack, "Ability.Attack", "기본 공격");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Attack, "GameplayCue.Attack", "공격 이펙트");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_ComboStart, "Event.Montage.Combo.Start", "콤보 시작");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_ComboEnd, "Event.Montage.Combo.End", "콤보 완료");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Dodge, "Ability.Dodge", "회피");
	
}