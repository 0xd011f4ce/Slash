// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Camera/CameraComponent.h"

#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "GroomComponent.h"

ASlashCharacter::ASlashCharacter ()
{
  PrimaryActorTick.bCanEverTick = true;

  bUseControllerRotationPitch = false;
  bUseControllerRotationYaw = false;
  bUseControllerRotationRoll = false;

  GetCharacterMovement ()->bOrientRotationToMovement = true;
  GetCharacterMovement ()->RotationRate = FRotator (0.f, 360.f, 0.f);

  SpringArm = CreateDefaultSubobject<USpringArmComponent> (TEXT ("SpringArm"));
  SpringArm->TargetArmLength = 300.f;
  SpringArm->SetupAttachment (GetRootComponent ());

  ViewCamera = CreateDefaultSubobject<UCameraComponent> (TEXT ("ViewCamera"));
  ViewCamera->SetupAttachment (SpringArm);

  Hair = CreateDefaultSubobject<UGroomComponent> (TEXT ("Hair"));
  Hair->AttachmentName = FString ("head");
  Hair->SetupAttachment (GetMesh ());

  Eyebrows = CreateDefaultSubobject<UGroomComponent> (TEXT ("Eyebrows"));
  Eyebrows->AttachmentName = FString ("head");
  Eyebrows->SetupAttachment (GetMesh ());
}

void
ASlashCharacter::BeginPlay ()
{
  Super::BeginPlay ();

  if (const APlayerController *PlayerController = Cast<APlayerController> (
      Controller))
    {
      if (UEnhancedInputLocalPlayerSubsystem *Subsystem =
          ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem> (
              PlayerController->GetLocalPlayer ()))
        {
          Subsystem->AddMappingContext (SlashContext, 0);
        }
    }
}

void
ASlashCharacter::Move (const FInputActionValue &Value)
{
  const FVector2D Direction = Value.Get<FVector2D> ();

  // find out which way is forward
  const FRotator ControlRotation = GetControlRotation ();
  const FRotator YawRotation (0.f, ControlRotation.Yaw, 0.f);

  const FVector ForwardDirection = FRotationMatrix (YawRotation).
      GetUnitAxis (EAxis::X);
  AddMovementInput (ForwardDirection, Direction.Y);

  // find out which way is right
  const FVector RightDirection = FRotationMatrix (YawRotation).
      GetUnitAxis (EAxis::Y);
  AddMovementInput (RightDirection, Direction.X);
}

void
ASlashCharacter::Turn (const FInputActionValue &Value)
{
  const FVector2D Direction = Value.Get<FVector2D> ();

  AddControllerYawInput (Direction.X);
  AddControllerPitchInput (Direction.Y);
}

void
ASlashCharacter::EquipPressed (const FInputActionValue &Value)
{
  AWeapon *OverlappingWeapon = Cast<AWeapon> (OverlappingItem);
  if (OverlappingItem)
    {
      OverlappingWeapon->Equip (GetMesh (), FName ("RightHandSocket"));
    }
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

  if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<
    UEnhancedInputComponent> (PlayerInputComponent))
    {
      EnhancedInputComponent->BindAction (MovementAction,
                                          ETriggerEvent::Triggered, this,
                                          &ASlashCharacter::Move);
      EnhancedInputComponent->BindAction (LookAction, ETriggerEvent::Triggered,
                                          this, &ASlashCharacter::Turn);
      EnhancedInputComponent->BindAction (JumpAction, ETriggerEvent::Triggered,
                                          this, &ACharacter::Jump);
      EnhancedInputComponent->BindAction (EquipAction,
                                          ETriggerEvent::Triggered,
                                          this,
                                          &ASlashCharacter::EquipPressed);
    }
}