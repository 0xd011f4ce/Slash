// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashCharacter.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ASlashCharacter::ASlashCharacter ()
{
  PrimaryActorTick.bCanEverTick = true;
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
    }
}
