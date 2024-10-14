// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Interfaces/HitInterface.h"

#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;

struct FInputActionValue;

UCLASS ()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
  GENERATED_BODY ()

public:
  ABaseCharacter ();
  virtual void Tick (float DeltaTime) override;

  UFUNCTION (BlueprintCallable)
  void SetWeaponCollisionEnabled (ECollisionEnabled::Type CollisionEnabled);

protected:
  virtual void BeginPlay () override;
  virtual void Attack ();
  virtual void Die ();

  virtual void PlayAttackMontage () const;
  void PlayHitReactMontage (const FName &SectionName);
  void DirectionalHitReact (const FVector &ImpactPoint);
  void PlayHitSound (const FVector &ImpactPoint);
  void SpawnHitParticles (const FVector &ImpactPoint);
  virtual void HandleDamage (float DamageAmount);

  UFUNCTION (BlueprintCallable)
  virtual void AttackEnd ();

  virtual bool CanAttack () const;
  bool IsAlive () const;

  UPROPERTY (VisibleAnywhere, Category = Weapon)
  AWeapon *EquippedWeapon;

  /**
   * Animation montages
   */
  UPROPERTY (EditDefaultsOnly, Category = Montages)
  UAnimMontage *AttackMontage;

  UPROPERTY (EditDefaultsOnly, Category = Montages)
  UAnimMontage *HitReactMontage;

  UPROPERTY (EditDefaultsOnly, Category = Montages)
  UAnimMontage *DeathMontage;

  /*
   * Components
   */
  UPROPERTY (VisibleAnywhere)
  UAttributeComponent *Attributes;

private:
  UPROPERTY (EditAnywhere, Category = Sound)
  USoundBase *HitSound;

  UPROPERTY (EditAnywhere, Category = VisualEffects)
  UParticleSystem *HitParticles;
};