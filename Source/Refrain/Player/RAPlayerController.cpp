// Fill out your copyright notice in the Description page of Project Settings.


#include "RAPlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/PauseMenu/RAPauseMenuWidget.h"
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
		// Unpause
		SetPause(false);

		if (UMagicalTimingSubsystem* TimingSubsystem = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>())
		{
			TimingSubsystem->ResumeMusic();
		}
		
		if (PauseMenuWidget)
		{
			PauseMenuWidget->RemoveFromParent();
		}
		
		FInputModeGameOnly GameOnlyInputMode;
		SetInputMode(GameOnlyInputMode);
		bShowMouseCursor = false;
	}
	else
	{
		// Pause
		SetPause(true);

		if (UMagicalTimingSubsystem* TimingSubsystem = GetWorld()->GetSubsystem<UMagicalTimingSubsystem>())
		{
			TimingSubsystem->PauseMusic();
		}
		
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
