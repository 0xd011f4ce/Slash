// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Characters/BaseCharacter.h"
#include "CoreMinimal.h"

#include "Interfaces/PickupInterface.h"

#include "CharacterTypes.h"

#include "SlashCharacter.generated.h"

class USlashOverlay;
class UInputMappingContext;
class UInputAction;
class UInputComponent;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class UAnimMontage;

class AItem;

UCLASS ()
class SLASH_API ASlashCharacter : public ABaseCharacter,
                                  public IPickupInterface
{
  GENERATED_BODY ()

public:
  ASlashCharacter ();

  virtual void SetupPlayerInputComponent (
      UInputComponent *PlayerInputComponent) override;

  virtual void GetHit_Implementation (const FVector &ImpactPoint,
                                      AActor *Hitter) override;
  virtual float TakeDamage (float DamageAmount,
                            const struct FDamageEvent &DamageEvent,
                            class AController *EventInstigator,
                            AActor *DamageCauser) override;

  virtual void Jump () override;

  virtual void SetOverlappingItem (AItem *Item) override;
  virtual void AddSouls (ASoul *Soul) override;
  virtual void AddGold (ATreasure *Treasure) override;

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
  virtual void Attack () override;

  /**
   * Combat
   */
  void EquipWeapon (AWeapon *Weapon);
  virtual void AttackEnd () override;
  virtual bool CanAttack () const override;
  void PlayEquipMontage (FName SectionName);
  bool CanDisarm () const;
  bool CanArm () const;
  void Disarm ();
  void Arm ();
  virtual void Die () override;

  UFUNCTION (BlueprintCallable)
  void AttachWeaponToBack ();

  UFUNCTION (BlueprintCallable)
  void AttachWeaponToHand ();

  UFUNCTION (BlueprintCallable)
  void FinishedEquipping ();

  UFUNCTION (BlueprintCallable)
  void HitReactEnd ();

private:
  bool Isunoccupied ();
  void InitialiseSlashOverlay ();
  void SetHUDHealth ();

  /*
   * Components
   */
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

  ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

  UPROPERTY (BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  EActionState ActionState = EActionState::EAS_Unoccupied;

  UPROPERTY ()
  USlashOverlay *SlashOverlay;

  // setters/getters
public:
  FORCEINLINE ECharacterState
  GetCharacterState () const
  {
    return CharacterState;
  }

  FORCEINLINE EActionState
  GetActionState () const
  {
    return ActionState;
  }
};