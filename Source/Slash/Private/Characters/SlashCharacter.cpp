// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Camera/CameraComponent.h"

#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"

#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Animation/AnimMontage.h"

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

  if (const APlayerController *PlayerController
      = Cast<APlayerController> (Controller))
    {
      if (UEnhancedInputLocalPlayerSubsystem *Subsystem
          = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem> (
              PlayerController->GetLocalPlayer ()))
        {
          Subsystem->AddMappingContext (SlashContext, 0);
        }
    }

  Tags.Add (FName ("SlashCharacter"));
}

void
ASlashCharacter::Move (const FInputActionValue &Value)
{
  if (ActionState != EActionState::EAS_Unoccupied)
    {
      return;
    }

  const FVector2D Direction = Value.Get<FVector2D> ();

  // find out which way is forward
  const FRotator ControlRotation = GetControlRotation ();
  const FRotator YawRotation (0.f, ControlRotation.Yaw, 0.f);

  const FVector ForwardDirection
      = FRotationMatrix (YawRotation).GetUnitAxis (EAxis::X);
  AddMovementInput (ForwardDirection, Direction.Y);

  // find out which way is right
  const FVector RightDirection
      = FRotationMatrix (YawRotation).GetUnitAxis (EAxis::Y);
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
      OverlappingWeapon->Equip (GetMesh (), FName ("RightHandSocket"), this,
                                this);
      CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;

      EquippedWeapon = OverlappingWeapon;
      OverlappingItem = nullptr;
    }
  else
    {
      // if we aren't overlapping a weapon, play the equip/unequip montage if
      // possible
      if (CanDisarm ())
        {
          PlayEquipMontage (FName ("Unequip"));
          CharacterState = ECharacterState::ECS_Unequipped;
          ActionState = EActionState::EAS_EquippingWeapon;
        }
      else if (CanArm ())
        {
          PlayEquipMontage (FName ("Equip"));
          CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
          ActionState = EActionState::EAS_EquippingWeapon;
        }
    }
}

void
ASlashCharacter::Attack ()
{
  Super::Attack ();

  if (CanAttack ())
    {
      PlayAttackMontage ();
      ActionState = EActionState::EAS_Attacking;
    }
}

void
ASlashCharacter::PlayEquipMontage (FName SectionName)
{
  UAnimInstance *AnimInstance = GetMesh ()->GetAnimInstance ();
  if (AnimInstance && EquipMontage)
    {
      AnimInstance->Montage_Play (EquipMontage);
      AnimInstance->Montage_JumpToSection (SectionName, EquipMontage);
    }
}

void
ASlashCharacter::AttackEnd ()
{
  ActionState = EActionState::EAS_Unoccupied;
}

bool
ASlashCharacter::CanAttack () const
{
  return ActionState == EActionState::EAS_Unoccupied
         && CharacterState != ECharacterState::ECS_Unequipped;
}

bool
ASlashCharacter::CanDisarm () const
{
  return ActionState == EActionState::EAS_Unoccupied
         && CharacterState != ECharacterState::ECS_Unequipped;
}

bool
ASlashCharacter::CanArm () const
{
  return ActionState == EActionState::EAS_Unoccupied
         && CharacterState == ECharacterState::ECS_Unequipped
         && EquippedWeapon;
}

void
ASlashCharacter::Disarm ()
{
  if (EquippedWeapon)
    {
      EquippedWeapon->AttachMeshToSocket (GetMesh (), FName ("SpineSocket"));
    }
}

void
ASlashCharacter::Arm ()
{
  if (EquippedWeapon)
    {
      EquippedWeapon->AttachMeshToSocket (GetMesh (),
                                          FName ("RightHandSocket"));
    }
}

void
ASlashCharacter::FinishedEquipping ()
{
  ActionState = EActionState::EAS_Unoccupied;
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

  if (UEnhancedInputComponent *EnhancedInputComponent
      = CastChecked<UEnhancedInputComponent> (PlayerInputComponent))
    {
      EnhancedInputComponent->BindAction (MovementAction,
                                          ETriggerEvent::Triggered, this,
                                          &ASlashCharacter::Move);
      EnhancedInputComponent->BindAction (LookAction, ETriggerEvent::Triggered,
                                          this, &ASlashCharacter::Turn);
      EnhancedInputComponent->BindAction (JumpAction, ETriggerEvent::Triggered,
                                          this, &ACharacter::Jump);
      EnhancedInputComponent->BindAction (EquipAction, ETriggerEvent::Started,
                                          this,
                                          &ASlashCharacter::EquipPressed);
      EnhancedInputComponent->BindAction (AttackAction,
                                          ETriggerEvent::Triggered, this,
                                          &ASlashCharacter::Attack);
    }
}