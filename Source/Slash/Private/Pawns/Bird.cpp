// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawns/Bird.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ABird::ABird ()
{
  PrimaryActorTick.bCanEverTick = true;

  Capsule = CreateDefaultSubobject<UCapsuleComponent> (TEXT ("Capsule"));
  Capsule->SetCapsuleHalfHeight (20.f);
  Capsule->SetCapsuleRadius (15.f);
  SetRootComponent (Capsule);

  BirdMesh
      = CreateDefaultSubobject<USkeletalMeshComponent> (TEXT ("BirdMesh"));
  BirdMesh->SetupAttachment (GetRootComponent ());

  AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void
ABird::BeginPlay ()
{
  Super::BeginPlay ();

  // setup the input context
  if (APlayerController *PlayerController
      = Cast<APlayerController> (GetController ()))
    {
      UEnhancedInputLocalPlayerSubsystem *LocalPlayerSubsystem
          = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem> (
              PlayerController->GetLocalPlayer ());
      if (LocalPlayerSubsystem)
        {
          LocalPlayerSubsystem->AddMappingContext (BirdMappingContext, 0);
        }
    }
}

void
ABird::Tick (float DeltaTime)
{
  Super::Tick (DeltaTime);
}

// Called to bind functionality to input
void
ABird::SetupPlayerInputComponent (UInputComponent *PlayerInputComponent)
{
  Super::SetupPlayerInputComponent (PlayerInputComponent);

  if (UEnhancedInputComponent *EnhancedInputComponent
      = CastChecked<UEnhancedInputComponent> (PlayerInputComponent))
    {
      EnhancedInputComponent->BindAction (MoveAction, ETriggerEvent::Triggered,
                                          this, &ABird::Move);
    }
}

void
ABird::Move (const FInputActionValue &Value)
{
  const float DirectionValue = Value.Get<float> ();

  if (Controller && (DirectionValue != 0.f))
    {
      FVector Forward = GetActorForwardVector ();
      AddMovementInput (Forward, DirectionValue);
    }
}