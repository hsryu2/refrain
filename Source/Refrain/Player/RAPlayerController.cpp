// Fill out your copyright notice in the Description page of Project Settings.


#include "RAPlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Refrain.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/RASaveScoreSubsystem.h"
#include "UI/PauseMenu/RAPauseMenuWidget.h"
#include "UI/RAScoreWidget.h"
#include "UI/Result/RAResultWidget.h"
#include "Timing/MagicalTimingSubsystem.h"
#include "UObject/ConstructorHelpers.h"
#include "Player/RAPlayerState.h"

ARAPlayerController::ARAPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Game/Refrain/Input/IMC_Player.IMC_Player"));

	if (InputMappingContextRef.Object != nullptr)
	{
		InputMappingContext = InputMappingContextRef.Object;
	}
}

void ARAPlayerController::InitHealthHUD(UAbilitySystemComponent* InASC)
{
	if (!IsLocalController() || !InASC || !HealthBarWidgetClass)
	{
		return;
	}
	
	if (!HealthBarWidget)
	{
		HealthBarWidget = CreateWidget<URAHealthBarWidget>(
			this,
			HealthBarWidgetClass
		);
		
		if (HealthBarWidget)
		{
			HealthBarWidget->AddToViewport();
			
			HealthBarWidget->SetAlignmentInViewport(FVector2D(0.0f, 0.0f));
			HealthBarWidget->SetPositionInViewport(FVector2D(40.0f, 40.0f));
			HealthBarWidget->SetDesiredSizeInViewport(FVector2D(300.0f, 28.0f));
		}
	}
	
	if (HealthBarWidget)
	{
		HealthBarWidget->InitAbilitySystem(InASC);
	}
}

void ARAPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerCameraManager)
	{
		PlayerCameraManager->ViewPitchMin = -45.0f;
		PlayerCameraManager->ViewPitchMax = 15.0f;
	}
	
	if (IsLocalController() && ScoreWidgetClass)
	{
		ScoreWidget = CreateWidget<URAScoreWidget>(this, ScoreWidgetClass);
		if (ScoreWidget)
		{
			ScoreWidget->AddToViewport();
		}
	}

	if (UMagicalTimingSubsystem* TimingSubsystem = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>())
	{
		TimingSubsystem->OnMusicFinished.AddUniqueDynamic(this, &ARAPlayerController::ShowResultUI);
	}

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}

void ARAPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &ARAPlayerController::TogglePauseMenu);
	}
}

void ARAPlayerController::TogglePauseMenu()
{
	if (UGameplayStatics::IsGamePaused(this))
	{
		if (PauseMenuWidget && PauseMenuWidget->IsCountingIn())
		{
			// 카운트인 중이면 취소하고 다시 메뉴 표시
			PauseMenuWidget->CancelCountIn();
			return;
		}
		
		if (PauseMenuWidget)
		{
			// 위젯이 있으면 카운트인 시작
			PauseMenuWidget->StartCountIn();
		}
		else
		{
			// 혹시 위젯이 없으면 바로 재개
			ExecuteUnpause();
		}
	}
	else
	{
		// Pause
		SetPause(true);

		// 음악 일시정지
		if (UMagicalTimingSubsystem* TimingSubsystem = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>())
		{
			TimingSubsystem->PauseMusic();
		}
		
		// Pause 메뉴 생성
		if (PauseMenuWidgetClass)
		{
			if (!PauseMenuWidget)
			{
				PauseMenuWidget = CreateWidget<URAPauseMenuWidget>(this, PauseMenuWidgetClass);
			}
			
			if (PauseMenuWidget)
			{
				if (!PauseMenuWidget->IsInViewport())
				{
					PauseMenuWidget->AddToViewport();
				}
			}
		}
		
		FInputModeGameAndUI GameAndUIInputMode;
		if (PauseMenuWidget)
		{
			GameAndUIInputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
		}
		SetInputMode(GameAndUIInputMode);
		bShowMouseCursor = true;
	}
}

void ARAPlayerController::ExecuteUnpause()
{
	// Unpause
	SetPause(false);

	// 음악 다시 재생
	if (UMagicalTimingSubsystem* TimingSubsystem = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>())
	{
		TimingSubsystem->ResumeMusic();
	}
	
	// Pause 메뉴 제거
	if (PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
	}
	
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
	bShowMouseCursor = false;
}

void ARAPlayerController::ShowResultUI()
{
	InternalShowEndGameUI(false);
}

void ARAPlayerController::ShowGameOverUI()
{
	InternalShowEndGameUI(true);
}

void ARAPlayerController::InternalShowEndGameUI(bool bIsGameOver)
{
	RA_LOG(LogRefrain, Log, TEXT("InternalShowEndGameUI Called! bIsGameOver: %d"), bIsGameOver);

	// 결과창이 뜰 때 기존 인게임 HUD(점수, 체력바)를 숨깁니다.
	FName CurrentSongID = NAME_None;
	if (UMagicalTimingSubsystem* TimingSubsystem = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>())
	{
		if (UMagicalMusicData* MusicData = TimingSubsystem->GetMusicData())
		{
			CurrentSongID = FName(*MusicData->SongTitle);
		}
	}
	int32 FinalScore = 0;
	if (ARAPlayerState* RAPS = GetPlayerState<ARAPlayerState>())
	{
		FinalScore = RAPS->TotalScore;
	}
	
	int32 OldHighScore = 0;
	if (CurrentSongID != NAME_None)
	{
		if (URASaveScoreSubsystem* ScoreSubsystem = GetGameInstance()->GetSubsystem<URASaveScoreSubsystem>())
		{
			// 위젯에 전달하기 위해 업데이트 전에 이전 최고 점수를 먼저 가져옵니다.
			OldHighScore = ScoreSubsystem->GetSongHighScore(CurrentSongID);
			
			// 최고 점수 갱신 및 저장
			ScoreSubsystem->UpdateAndSaveSongHighScore(CurrentSongID, FinalScore);
		}
	}

	
	// 결과창이 뜰 때 기존 인게임 HUD(점수, 체력바)를 숨김처리
	if (ScoreWidget)
	{
		ScoreWidget->RemoveFromParent();
	}
	if (HealthBarWidget)
	{
		HealthBarWidget->RemoveFromParent();
	}

	if (ResultWidgetClass)
	{
		if (!ResultWidget)
		{
			ResultWidget = CreateWidget<URAResultWidget>(this, ResultWidgetClass);
			RA_LOG(LogRefrain, Log, TEXT("ResultWidget created."));
		}
		
		if (ResultWidget)
		{
			ResultWidget->bIsGameOver = bIsGameOver;
			ResultWidget->PreviousHighScore = OldHighScore;
			ResultWidget->bIsNewRecord = (FinalScore > OldHighScore);
			ResultWidget->SetScoreVisibility(!bIsGameOver);
			
			if (!ResultWidget->IsInViewport())
			{
				ResultWidget->AddToViewport();
				RA_LOG(LogRefrain, Log, TEXT("ResultWidget added to viewport."));
			}
		}
	}
	else
	{
		RA_LOG(LogRefrain, Error, TEXT("ResultWidgetClass is NULL! 블루프린트에서 위젯 클래스를 설정했는지 확인하세요."));
	}
	
	FInputModeGameAndUI GameAndUIInputMode;
	if (ResultWidget)
	{
		GameAndUIInputMode.SetWidgetToFocus(ResultWidget->TakeWidget());
	}
	SetInputMode(GameAndUIInputMode);
	bShowMouseCursor = true;

	SetPause(true);
}

