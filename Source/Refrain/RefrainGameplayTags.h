#pragma once

#include "NativeGameplayTags.h"

namespace RefrainGameplayTags
{
	// Attack 태그.
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack);
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Attack);
	
	// AttackCombo 시작, 완료 태그
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_ComboInputStart);
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_ComboInputEnd);
	
	// Dodge 태그.
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dodge);
	
	// 데미지 관련 태그
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_AttackHit);
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage);
	
	// 캐릭터 상태
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Untargetable);
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_HitReact);
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);

	// 사망처리
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Dissolve);
	
	// 점수 판정
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Judge_Perfect);
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Judge_Good);
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Judge_Bad);
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Judge_None);
	
	// 공격 애니메이션 재생 속도 조절에 사용되는 태그
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_PlayRate);
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_PlayRate_StartupToAnticipation);
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_PlayRate_AnticipationToStrike);
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_PlayRate_StrikeToRecovery);
	
	// 다음 콤보로 넘어가는 이벤트
	REFRAIN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_NextComboStart);

	
}
