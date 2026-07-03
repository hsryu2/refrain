// Fill out your copyright notice in the Description page of Project Settings.


#include "RAPlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/PauseMenu/RAPauseMenuWidget.h"
#include "UI/RAScoreWidget.h"
#include "UI/Result/RAResultWidget.h"
#include "Timing/MagicalTimingSubsystem.h"
#include "UObject/ConstructorHelpers.h"

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

	if (IsLocalController() && ScoreWidgetClass)
	{
		ScoreWidget = CreateWidget<URAScoreWidget>(this, ScoreWidgetClass);
		if (ScoreWidget)
		{
			ScoreWidget->AddToViewport();
		}
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
	if (ResultWidgetClass)
	{
		if (!ResultWidget)
		{
			ResultWidget = CreateWidget<URAResultWidget>(this, ResultWidgetClass);
		}
		
		if (ResultWidget)
		{
			if (!ResultWidget->IsInViewport())
			{
				ResultWidget->AddToViewport();
			}
		}
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

