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

protected:
  virtual void BeginPlay () override;
  virtual void GetHit_Implementation (const FVector &ImpactPoint, AActor *Hitter) override;

  virtual void Attack ();
  virtual void Die ();
  void DirectionalHitReact (const FVector &ImpactPoint);
  virtual void HandleDamage (float DamageAmount);
  void PlayHitSound (const FVector &ImpactPoint);
  void SpawnHitParticles (const FVector &ImpactPoint);
  void DisableCapsule ();

  virtual int32 PlayAttackMontage ();
  virtual int32 PlayDeathMontage ();
  void PlayHitReactMontage (const FName &SectionName);
  virtual bool CanAttack () const;
  bool IsAlive () const;

  UFUNCTION (BlueprintCallable)
  virtual void AttackEnd ();

  UFUNCTION (BlueprintCallable)
  void SetWeaponCollisionEnabled (ECollisionEnabled::Type CollisionEnabled);

  UPROPERTY (VisibleAnywhere, Category = Weapon)
  AWeapon *EquippedWeapon;

  /*
   * Components
   */
  UPROPERTY (VisibleAnywhere)
  UAttributeComponent *Attributes;

private:
  UPROPERTY (EditAnywhere, Category = Combat)
  USoundBase *HitSound;

  UPROPERTY (EditAnywhere, Category = Combat)
  UParticleSystem *HitParticles;

  /**
   * Animation montages
   */
  void PlayMontageSection (UAnimMontage *Montage, const FName &SectionName);
  int32 PlayRandomMontageSection (UAnimMontage *Montage,
                                  const TArray<FName> &SectionNames);

  UPROPERTY (EditDefaultsOnly, Category = Montages)
  UAnimMontage *AttackMontage;

  UPROPERTY (EditDefaultsOnly, Category = Montages)
  UAnimMontage *HitReactMontage;

  UPROPERTY (EditDefaultsOnly, Category = Montages)
  UAnimMontage *DeathMontage;

  UPROPERTY (EditAnywhere, Category = Combat)
  TArray<FName> AttackMontageSections;

  UPROPERTY (EditAnywhere, Category = Combat)
  TArray<FName> DeathMontageSections;
};