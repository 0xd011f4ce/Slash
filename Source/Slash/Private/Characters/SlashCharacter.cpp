// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ASlashCharacter::ASlashCharacter ()
{
  PrimaryActorTick.bCanEverTick = true;

  bUseControllerRotationPitch = false;
  bUseControllerRotationYaw = false;
  bUseControllerRotationRoll = false;

  SpringArm = CreateDefaultSubobject <USpringArmComponent> (TEXT ("SpringArm"));
  SpringArm->TargetArmLength = 300.f;
  SpringArm->SetupAttachment (GetRootComponent ());

  ViewCamera = CreateDefaultSubobject <UCameraComponent> (TEXT ("ViewCamera"));
  ViewCamera->SetupAttachment (SpringArm);
}

void
ASlashCharacter::BeginPlay ()
{
  Super::BeginPlay ();

  if (APlayerController *PlayerController = Cast<APlayerController> (Controller))
    {
      if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer ()))
        {
          Subsystem->AddMappingContext (SlashContext, 0);
        }
    }
}

void
ASlashCharacter::Move (const FInputActionValue &Value)
{
  const FVector2D Direction = Value.Get<FVector2D> ();

  const FVector Forward = GetActorForwardVector ();
  AddMovementInput (Forward, Direction.Y);

  const FVector Right = GetActorRightVector ();
  AddMovementInput (Right, Direction.X);
}

void
ASlashCharacter::Turn (const FInputActionValue &Value)
{
  const FVector2D Direction = Value.Get<FVector2D> ();

  AddControllerYawInput (Direction.X);
  AddControllerPitchInput (Direction.Y);
}

void
ASlashCharacter::Tick (float DeltaTime)
{
  Super::Tick (DeltaTime);
}

void
ASlashCharacter::SetupPlayerInputComponent (
    UInputComponent *PlayerInputComponent)
{
  Super::SetupPlayerInputComponent (PlayerInputComponent);

  if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent> (PlayerInputComponent))
    {
      EnhancedInputComponent->BindAction (MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
      EnhancedInputComponent->BindAction (LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Turn);
    }
}
