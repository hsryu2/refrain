#include "RefrainGameplayTags.h"

namespace RefrainGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack, "Ability.Attack", "기본 공격");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Attack, "GameplayCue.Attack", "공격 이펙트");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_Counter, "Ability.Attack.Counter", "카운터 공격");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_ComboInputStart, "Event.Montage.ComboInputStart", "콤보 시작");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_ComboInputEnd, "Event.Montage.ComboInputEnd", "콤보 완료");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Dodge, "Ability.Dodge", "회피");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_AttackHit, "Event.Montage.AttackHit", "공격 적중");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_Damage, "Data.Damage", "데미지");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Untargetable, "State.Untargetable", "대상 지정 불가");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_HitReact, "State.HitReact", "피격 중인 상태");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Dead, "State.Dead", "사망 상태");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Dodging, "State.Dodge", "회피 중");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Attacking, "State.Attacking", "공격 중");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Dissolve, "GameplayCue.Dissolve", "소멸 이펙트");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Judge_Perfect, "Judge.Perfect", "Perfect 판정");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Judge_Good, "Judge.Good", "Good 판정");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Judge_Bad, "Judge.Bad", "Bad 판정");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Judge_Miss, "Judge.Miss", "공격 대상이 없을 때");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_PlayRate, "Event.Montage.PlayRate", "하위 태그들 공통으로 이벤트 호출하기 위한 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_PlayRate_StartupToAnticipation, "Event.Montage.PlayRate.StartupToAnticipation", "공격 준비 모션이 드러나기 시작하는 시점");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_PlayRate_AnticipationToStrike, "Event.Montage.PlayRate.AnticipationToStrike", "본격적인 타격 모션이 시작되는 시점");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_PlayRate_StrikeToRecovery, "Event.Montage.PlayRate.StrikeToRecovery", "타격 모션이 끝나고 후딜이 시작되는 시점");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_AttackHit_FirstHit, "Event.Montage.AttackHit.FirstHit", "다중 타격 몽타주에서 첫 번째 공격 시점");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_AttackHit_SecondHit, "Event.Montage.AttackHit.SecondHit", "다중 타격 몽타주에서 두 번째 공격 시점");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_NextComboStart, "Event.Montage.NextComboStart", "다음 콤보가 예약되어 있을 경우 다음 콤보로 넘어가는 시점");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Attacking_Main, "State.Attacking.Main", "공격 중인 상태");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Attacking_Counterable, "State.Attacking.Counterable", "카운터 가능한 공격");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Attacking_Counterable_InWindow, "State.Attacking.Counterable.InWindow", "카운터 가능한 공격의 카운터 가능한 시점");
}
