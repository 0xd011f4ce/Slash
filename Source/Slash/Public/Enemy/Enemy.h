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

  /* <AActor> */
  virtual void Tick (float DeltaTime) override;
  virtual float TakeDamage (float DamageAmount,
                            const struct FDamageEvent &DamageEvent,
                            class AController *EventInstigator,
                            AActor *DamageCauser) override;
  virtual void Destroyed () override;
  /* </AActor> */

  /* <IHitInterface> */
  virtual void GetHit_Implementation (const FVector &ImpactPoint) override;
  /* </IHitInterface> */

protected:
  /* <AActor> */
  virtual void BeginPlay () override;
  /* </AActor> */

  /* <ABaseCharacter> */
  virtual void Die () override;
  virtual void Attack () override;
  virtual bool CanAttack () const override;
  virtual void AttackEnd () override;
  virtual void HandleDamage (float DamageAmount) override;
  virtual int32 PlayDeathMontage () override;
  /* </ABaseCharacter> */

  UPROPERTY (BlueprintReadOnly)
  TEnumAsByte<EDeathPose> DeathPose;

  UPROPERTY (BlueprintReadOnly)
  EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:
  /* AI Behaviour */
  void InitialiseEnemy ();
  void CheckPatrolTarget ();
  void CheckCombatTarget ();
  void PatrolTimerFinished ();
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
  bool InTargetRange (AActor *Target, double Radius) const;
  void ClearPatrolTimer ();
  void StartAttackTimer ();
  void ClearAttackTimer ();
  void MoveToTarget (const AActor *Target) const;
  AActor *ChoosePatrolTarget ();

  void SpawnDefaultWeapon ();

  UFUNCTION ()
  void PawnSeen (APawn *SeenPawn); // PawnSensingComponent callback

  UPROPERTY (VisibleAnywhere)
  UHealthBarComponent *HealthBarWidget;

  UPROPERTY (VisibleAnywhere)
  UPawnSensingComponent *PawnSensing;

  UPROPERTY (EditAnywhere)
  TSubclassOf<AWeapon> WeaponClass;

  UPROPERTY ()
  AActor *CombatTarget;

  UPROPERTY (EditAnywhere)
  double CombatRadius = 1000.f;

  UPROPERTY (EditAnywhere)
  double AttackRadius = 150.f;

  UPROPERTY (EditAnywhere)
  double PatrolRadius = 200.f;

  FTimerHandle PatrolTimer;

  UPROPERTY (EditAnywhere, Category = "AI Navigation")
  float PatrolWaitMin = 5.f;

  UPROPERTY (EditAnywhere, Category = "AI Navigation")
  float PatrolWaitMax = 10.f;

  UPROPERTY (EditAnywhere, Category = Combat)
  float PatrollingSpeed = 150.f;

  FTimerHandle AttackTimer;

  UPROPERTY (EditAnywhere, Category = Combat)
  float AttackMin = 0.5f;

  UPROPERTY (EditAnywhere, Category = Combat)
  float AttackMax = 1.f;

  UPROPERTY (EditAnywhere, Category = Combat)
  float ChasingSpeed = 300.f;

  UPROPERTY (EditAnywhere, Category = Combat)
  float DeathLifeSpan = 8.f;

  UPROPERTY ()
  AAIController *EnemyController;

  UPROPERTY (EditInstanceOnly, Category = "AI Navigation")
  AActor *PatrolTarget;

  UPROPERTY (EditInstanceOnly, Category = "AI Navigation")
  TArray<AActor *> PatrolTargets;
};