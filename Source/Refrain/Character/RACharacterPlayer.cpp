// Fill out your copyright notice in the Description page of Project Settings.


#include "RACharacterPlayer.h"

#include "AbilitySystemComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Refrain/Animation/RACharacterAnimationData.h"
#include "Refrain/Component/AttackTargetingComponent.h"
#include "Refrain/Player/RAPlayerState.h"
#include "Component/AttackTargetingComponent.h"

// Sets default values
ARACharacterPlayer::ARACharacterPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	
	// Camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bUsePawnControlRotation = true;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = true;
	

	// Input
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultContextRef(TEXT("/Game/Refrain/Input/IMC_Player.IMC_Player"));
	if (DefaultContextRef.Succeeded())
	{
		DefaultContext = DefaultContextRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionMoveRef(TEXT("/Game/Refrain/Input/InputAction/IA_Move.IA_Move"));
	if (InputActionMoveRef.Succeeded())
	{
		MoveAction = InputActionMoveRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionLookRef(TEXT("/Game/Refrain/Input/InputAction/IA_Look.IA_Look"));
	if (InputActionLookRef.Succeeded())
	{
		LookAction = InputActionLookRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAttackRef(TEXT("/Game/Refrain/Input/InputAction/IA_Attack.IA_Attack"));
	if (InputActionAttackRef.Succeeded())
	{
		AttackAction = InputActionAttackRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionDodgeRef(TEXT("/Game/Refrain/Input/InputAction/IA_Dodge.IA_Dodge"));
	if (InputActionAttackRef.Succeeded())
	{
		DodgeAction = InputActionDodgeRef.Object;
	}
	
	// GAS
	ASC = nullptr;
	
	// 컴포넌트
	TargetingComponent = CreateDefaultSubobject<UAttackTargetingComponent>(TEXT("TargetingComponent"));
}

class UAbilitySystemComponent* ARACharacterPlayer::GetAbilitySystemComponent() const
{
	return ASC;
}

// Called when the game starts or when spawned
void ARACharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	SetIMC();
}

// Called every frame
void ARACharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARACharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	ARAPlayerState* GASPS = GetPlayerState<ARAPlayerState>();
	if (GASPS)
	{
		ASC = GASPS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(GASPS, this);
		
		for (const auto& StartAbility : StartAbilities)
		{
			FGameplayAbilitySpec StartSpec(StartAbility);
			ASC->GiveAbility(StartSpec);
		}

		for (const auto& StartInputAbility : StartInputAbilities)
		{
			FGameplayAbilitySpec StartSpec(StartInputAbility.Value);
			StartSpec.InputID = StartInputAbility.Key;
			ASC->GiveAbility(StartSpec);
		}

		//SetupGASInputComponent();

		//APlayerController* PlayerController = CastChecked<APlayerController>(NewController);
		//PlayerController->ConsoleCommand(TEXT("showdebug abilitysystem"));
	}
}
void ARACharacterPlayer::SetupGASInputComponent()
{
	if (IsValid(ASC) && IsValid(InputComponent))
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
		
		// GAS로 만들 플레이어 액션 여기에 추가.
		// (GetInputPressed, InputId)로 추가.
		EnhancedInputComponent->BindAction(
			AttackAction, ETriggerEvent::Triggered, this, &ARACharacterPlayer::GASInputPressed, 0);
		EnhancedInputComponent->BindAction(
			DodgeAction, ETriggerEvent::Triggered, this, &ARACharacterPlayer::GASInputPressed, 1);
	}
}

void ARACharacterPlayer::GASInputPressed(int32 InputId)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputId);
	if (Spec)
	{
		Spec->InputPressed = true;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputPressed(*Spec);
		}
		else
		{
			ASC->TryActivateAbility(Spec->Handle);
		}
	}
}

void ARACharacterPlayer::GASInputReleased(int32 InputId)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputId);
	if (Spec)
	{
		Spec->InputPressed = false;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*Spec);
		}
	}
}

// Called to bind functionality to input
void ARACharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	
	EnhancedInputComponent->BindAction(
		MoveAction,
		ETriggerEvent::Triggered,
		this,
		&ARACharacterPlayer::Move
	);
	
	EnhancedInputComponent->BindAction(
		LookAction,
		ETriggerEvent::Triggered,
		this,
		&ARACharacterPlayer::Look
	);
	
	SetupGASInputComponent();
}

// IMC 설정
void ARACharacterPlayer::SetIMC()
{
	// 입력 매핑 컨텍스트 설정.
	APlayerController* PlayerController
		= Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		// 향상된 입력 시스템의 서브 시스템 가져오기.
		UEnhancedInputLocalPlayerSubsystem* InputSystem
			= ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				PlayerController->GetLocalPlayer()
			);
		if (InputSystem)
		{
			InputSystem->ClearAllMappings();

			// 매핑 컨텍스트 추가 (우선순위 0)
			InputSystem->AddMappingContext(DefaultContext, 0);
		}

	}
}

UAnimMontage* ARACharacterPlayer::GetAttackMontage(int32 ComboIndex) const
{
	if (!AnimationData)
	{
		return nullptr;
	}
	switch (ComboIndex)
	{
	case 1:
		return AnimationData->AttackMontage_1;
	case 2:
		return AnimationData->AttackMontage_2;
	case 3:
		return AnimationData->AttackMontage_3;
	default:
		return nullptr;
	}

}

UAnimMontage* ARACharacterPlayer::GetDodgeMontage() const
{
	return AnimationData->DodgeMontage;
}

void ARACharacterPlayer::Move(const FInputActionValue& Value)
{
	// 입력값 = Vector
	FVector Movement = Value.Get<FVector>();

	// 이동할 방향 -> Rotator
	FRotator Rotation = GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);


	// 앞 방향.
	FVector ForwardVector
		= FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	// 오른쪽 방향.
	FVector RightVector
		= FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardVector, Movement.Y);
	AddMovementInput(RightVector, Movement.X);
}

void ARACharacterPlayer::Look(const FInputActionValue& Value)
{
	// 입력값 가져오기.
	FVector2D RotationValue = Value.Get<FVector2D>();
	// 회전 처리
	AddControllerYawInput(RotationValue.X * 0.7);

	// 마우스를 올리면 위로 보도록 -1을 곱함.
	AddControllerPitchInput((RotationValue.Y) * 0.5);
}

