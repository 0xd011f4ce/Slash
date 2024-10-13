// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Characters/BaseCharacter.h"
#include "CoreMinimal.h"

#include "InputActionValue.h"

#include "CharacterTypes.h"

#include "SlashCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class UAnimMontage;

class AItem;

UCLASS ()
class SLASH_API ASlashCharacter : public ABaseCharacter
{
  GENERATED_BODY ()

public:
  ASlashCharacter ();

  virtual void Tick (float DeltaTime) override;

  virtual void SetupPlayerInputComponent (
      class UInputComponent *PlayerInputComponent) override;

protected:
  virtual void BeginPlay () override;

  UPROPERTY (EditAnywhere, Category = Input)
  UInputMappingContext *SlashContext;

  UPROPERTY (EditAnywhere, Category = Input)
  UInputAction *MovementAction;

  UPROPERTY (EditAnywhere, Category = Input)
  UInputAction *LookAction;

  UPROPERTY (EditAnywhere, Category = Input)
  UInputAction *JumpAction;

  UPROPERTY (EditAnywhere, Category = Input)
  UInputAction *EquipAction;

  UPROPERTY (EditAnywhere, Category = Input)
  UInputAction *AttackAction;

  /**
   * Callbacks for input
   */
  void Move (const FInputActionValue &Value);
  void Turn (const FInputActionValue &Value);
  void EquipPressed (const FInputActionValue &Value);
  virtual void Attack (const FInputActionValue &Value) override;

  /**
   * Play Montage Functions
   */
  virtual void PlayAttackMontage () const override;

  virtual void AttackEnd () override;
  virtual bool CanAttack () const override;

  void PlayEquipMontage (FName SectionName);
  bool CanDisarm () const;
  bool CanArm () const;

  UFUNCTION (BlueprintCallable)
  void Disarm ();

  UFUNCTION (BlueprintCallable)
  void Arm ();

  UFUNCTION (BlueprintCallable)
  void FinishedEquipping ();

private:
  ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

  UPROPERTY (BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  EActionState ActionState = EActionState::EAS_Unoccupied;

  UPROPERTY (VisibleAnywhere)
  USpringArmComponent *SpringArm;

  UPROPERTY (VisibleAnywhere)
  UCameraComponent *ViewCamera;

  UPROPERTY (VisibleAnywhere, Category = Hair)
  UGroomComponent *Hair;

  UPROPERTY (VisibleAnywhere, Category = Hair)
  UGroomComponent *Eyebrows;

  UPROPERTY (VisibleInstanceOnly)
  AItem *OverlappingItem;

  /**
   * Animation montages
   */
  UPROPERTY (EditDefaultsOnly, Category = Montages)
  UAnimMontage *EquipMontage;

  // setters/getters
public:
  FORCEINLINE void
  SetOverlappingItem (AItem *Item)
  {
    OverlappingItem = Item;
  }

  FORCEINLINE ECharacterState
  GetCharacterState () const
  {
    return CharacterState;
  }
};