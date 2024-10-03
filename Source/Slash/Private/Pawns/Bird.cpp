// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawns/Bird.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Camera/CameraComponent.h"

#include "GameFramework/SpringArmComponent.h"

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

  SpringArm = CreateDefaultSubobject<USpringArmComponent> (TEXT ("SpringArm"));
  SpringArm->TargetArmLength = 300.f;
  SpringArm->SetupAttachment (GetRootComponent ());

  ViewCamera = CreateDefaultSubobject<UCameraComponent> (TEXT ("ViewCamera"));
  ViewCamera->SetupAttachment (SpringArm);

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
      EnhancedInputComponent->BindAction (LookAction, ETriggerEvent::Triggered,
                                          this, &ABird::Look);
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

void
ABird::Look (const FInputActionValue &Value)
{
  const FVector2D LookValue = Value.Get<FVector2D>();
  if (GetController ())
    {
      AddControllerYawInput (LookValue.X);
      AddControllerPitchInput (LookValue.Y);
    }
}