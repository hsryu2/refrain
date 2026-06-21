// Fill out your copyright notice in the Description page of Project Settings.


#include "RACharacterNonPlayer.h"
#include "Components/WidgetComponent.h"
#include "Refrain/UI/RhythmTargetWidget.h"

ARACharacterNonPlayer::ARACharacterNonPlayer()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	PrimaryActorTick.bCanEverTick = true;

	// 위젯 컴포넌트 생성 및 루트(또는 메쉬)에 부착
	RhythmTargetWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("RhythmWidgetComponent"));
	RhythmTargetWidget->SetupAttachment(RootComponent);
	
	// 수동 DrawSize를 없애는 대신 블루프린트에서 제어 가능하도록 함
	RhythmTargetWidget->SetDrawAtDesiredSize(false);
	
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AttributeSet = CreateDefaultSubobject<URAAttributeSet>(TEXT("AttributeSet"));
	
}

UAbilitySystemComponent* ARACharacterNonPlayer::GetAbilitySystemComponent() const
{
	return ASC;
}

void ARACharacterNonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (RhythmTargetWidget)
	{
		// 실제 게임 플레이가 시작되면 다시 Screen 공간으로 되돌려 UI 본연의 역할을 하게 합니다.
		if (GetWorld() && GetWorld()->IsGameWorld())
		{
			// Screen 공간에서는 스케일이 다시 1.0이어야 모니터 픽셀에 1:1로 정확히 맞습니다.
			RhythmTargetWidget->SetWidgetSpace(EWidgetSpace::Screen);
			RhythmTargetWidget->SetDrawSize(WidgetDrawSize);
			RhythmTargetWidget->SetRelativeScale3D(FVector(1.0f));		
		}
		
		// 위젯 컴포넌트로부터 실제 유저 위젯 인스턴스를 가져와 캐싱합니다.
		RhythmWidgetInstance = Cast<URhythmTargetWidget>(RhythmTargetWidget->GetUserWidgetObject());
	}
}

void ARACharacterNonPlayer::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateWidgetPreview();
}

void ARACharacterNonPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);
	}
}

void ARACharacterNonPlayer::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
    
	// 변수가 수정되면 즉시 프리뷰 업데이트
	UpdateWidgetPreview();
}

void ARACharacterNonPlayer::UpdateWidgetPreview()
{
	if (RhythmTargetWidget)
	{
		// 에디터(비게임 월드) 환경에서만 World Space로 프리뷰 제공
		bool bIsEditor = GetWorld() && !GetWorld()->IsGameWorld();
        
		if (bIsEditor)
		{
			RhythmTargetWidget->SetWidgetSpace(EWidgetSpace::World);
			RhythmTargetWidget->SetDrawSize(WidgetDrawSize);
			RhythmTargetWidget->SetRelativeScale3D(FVector(WidgetEditorScale));
		}
	}
}
