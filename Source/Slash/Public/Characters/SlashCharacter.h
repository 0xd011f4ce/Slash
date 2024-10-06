// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "InputActionValue.h"

#include "SlashCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;

class AItem;

UCLASS ()
class SLASH_API ASlashCharacter : public ACharacter
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

  void Move (const FInputActionValue &Value);
  void Turn (const FInputActionValue &Value);
  void EquipPressed (const FInputActionValue &Value);

private:
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

  // setters/getters
public:
  FORCEINLINE void
  SetOverlappingItem (AItem *Item) { OverlappingItem = Item; }
};