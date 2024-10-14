// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Characters/BaseCharacter.h"
#include "CoreMinimal.h"

#include "Characters/CharacterTypes.h"

#include "Enemy.generated.h"

class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;
class AWeapon;

UCLASS ()
class SLASH_API AEnemy : public ABaseCharacter
{
  GENERATED_BODY ()

public:
  AEnemy ();
  virtual void Tick (float DeltaTime) override;
  void CheckPatrolTarget ();
  void CheckCombatTarget ();

  virtual void GetHit_Implementation (const FVector &ImpactPoint) override;
  virtual float TakeDamage (float DamageAmount,
                            const struct FDamageEvent &DamageEvent,
                            class AController *EventInstigator,
                            AActor *DamageCauser) override;

  virtual void Destroyed () override;

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay () override;

  virtual void Die () override;
  bool InTargetRange (AActor *Target, double Radius) const;
  void MoveToTarget (AActor *Target);
  AActor *ChoosePatrolTarget ();
  virtual void Attack () override;
  virtual void PlayAttackMontage () const override;
  virtual bool CanAttack () const override;
  virtual void HandleDamage (float DamageAmount) override;

  UFUNCTION ()
  void PawnSeen (APawn *SeenPawn);

  UPROPERTY (BlueprintReadOnly)
  EDeathPose DeathPose;

  UPROPERTY (BlueprintReadOnly)
  EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:
  UPROPERTY (VisibleAnywhere)
  UHealthBarComponent *HealthBarWidget;

  UPROPERTY (VisibleAnywhere)
  UPawnSensingComponent *PawnSensing;

  UPROPERTY ()
  AActor *CombatTarget;

  UPROPERTY (EditAnywhere)
  double CombatRadius = 500.f;

  UPROPERTY (EditAnywhere)
  double AttackRadius = 150.f;

  UPROPERTY (EditAnywhere)
  double PatrolRadius = 200.f;

  UPROPERTY (EditAnywhere)
  TSubclassOf<AWeapon> WeaponClass;

  /*
   * Navigation
   */
  // Current patrol target
  UPROPERTY ()
  AAIController *EnemyController;

  UPROPERTY (EditInstanceOnly, Category = "AI Navigation")
  AActor *PatrolTarget;

  UPROPERTY (EditInstanceOnly, Category = "AI Navigation")
  TArray<AActor *> PatrolTargets;

  FTimerHandle PatrolTimer;
  void PatrolTimerFinished ();

  UPROPERTY (EditAnywhere, Category = "AI Navigation")
  float WaitMin = 5.f;

  UPROPERTY (EditAnywhere, Category = "AI Navigation")
  float WaitMax = 10.f;

  /*
   * AI Behaviour
   */
  void HideHealthBar ();
  void ShowHealthBar ();
  void LoseInterest ();
  void StartPatrolling ();
  void ChaseTarget ();
  bool IsOutsideCombatRadius () const;
  bool IsOutsideAttackRadius () const;
  bool IsInsideAttackRadius () const;
  bool IsChasing () const;
  bool IsAttacking () const;
  bool IsDead () const;
  bool IsEngaged () const;
  void ClearPatrolTimer ();

  /*
   * Combat
   */
  void StartAttackTimer ();
  void ClearAttackTimer ();

  FTimerHandle AttackTimer;

  UPROPERTY (EditAnywhere, Category = Combat)
  float AttackMin = 0.5f;

  UPROPERTY (EditAnywhere, Category = Combat)
  float AttackMax = 1.f;

  UPROPERTY (EditAnywhere, Category = Combat)
  float PatrollingSpeed = 150.f;

  UPROPERTY (EditAnywhere, Category = Combat)
  float ChasingSpeed = 300.f;
};