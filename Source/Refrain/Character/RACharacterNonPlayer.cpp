// Fill out your copyright notice in the Description page of Project Settings.


#include "RACharacterNonPlayer.h"
#include "Components/WidgetComponent.h"
#include "Refrain/UI/RhythmTargetWidget.h"
#include "AbilitySystemComponent.h"

ARACharacterNonPlayer::ARACharacterNonPlayer()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	PrimaryActorTick.bCanEverTick = true;

	// 위젯 컴포넌트 생성 및 루트(또는 메쉬)에 부착
	RhythmTargetWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("RhythmWidgetComponent"));
	RhythmTargetWidget->SetupAttachment(RootComponent);
	
	// 수동 DrawSize를 없애는 대신 블루프린트에서 제어 가능하도록 함
	RhythmTargetWidget->SetDrawAtDesiredSize(false);
	
	// GAS
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AttributeSet = CreateDefaultSubobject<URAAttributeSet>(TEXT("AttributeSet"));
	
	if (RhythmTargetWidget)
	{
		RhythmTargetWidget->SetVisibility(false);
	}
}

UAbilitySystemComponent* ARACharacterNonPlayer::GetAbilitySystemComponent() const
{
	return ASC;
}

void ARACharacterNonPlayer::SetRhythmWidgetVisibility(bool bShow)
{
	if (RhythmTargetWidget)
	{
		RhythmTargetWidget->SetVisibility(bShow);
	}
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
		// 1. ASC의 ActorInfo를 먼저 초기화
		ASC->InitAbilityActorInfo(this, this);
		
		// 2. 초기화용 Gameplay Effect가 블루프린트에 등록되어 있다면 자신에게 적용.
		if (InitStatEffect)
		{
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			ContextHandle.AddInstigator(this, this);

			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(InitStatEffect, 1.0f, ContextHandle);
			if (SpecHandle.IsValid())
			{
				// 포인터 역참조를 통해 Spec 데이터를 전달하여 자신에게 이펙트를 적용.
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
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

void ARACharacterNonPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);

		for (const auto& StartAbility : StartAbilities)
		{
			FGameplayAbilitySpec StartSpec(StartAbility);
			ASC->GiveAbility(StartSpec);
		}
	}
}