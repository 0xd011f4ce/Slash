// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"

#include "Components/AttributeComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "HUD/HealthBarComponent.h"

#include "Animation/AnimMontage.h"

#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

AEnemy::AEnemy ()
{
  PrimaryActorTick.bCanEverTick = true;

  GetMesh ()->SetCollisionObjectType (ECC_WorldDynamic);
  GetMesh ()->SetCollisionResponseToChannel (ECC_Visibility,
                                             ECR_Block);
  GetMesh ()->SetCollisionResponseToChannel (ECC_Camera,
                                             ECR_Ignore);
  GetMesh ()->SetGenerateOverlapEvents (true);

  // initialise components
  HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent> (
      TEXT ("HealthBarWidgetComponent"));
  HealthBarWidget->SetupAttachment (GetRootComponent ());

  GetCharacterMovement ()->bOrientRotationToMovement = true;
  bUseControllerRotationPitch = false;
  bUseControllerRotationYaw = false;
  bUseControllerRotationRoll = false;

  PawnSensing
      = CreateDefaultSubobject<UPawnSensingComponent> (TEXT ("Pawn Sensing"));
  PawnSensing->SetPeripheralVisionAngle (45.f);
  PawnSensing->SightRadius = 4000.f;
}

void
AEnemy::Tick (float DeltaTime)
{
  Super::Tick (DeltaTime);

  if (IsDead ())
    {
      return;
    }

  if (EnemyState > EEnemyState::EES_Patrolling)
    {
      CheckCombatTarget ();
    }
  else
    {
      CheckPatrolTarget ();
    }
}

float
AEnemy::TakeDamage (float DamageAmount, const FDamageEvent &DamageEvent,
                    AController *EventInstigator, AActor *DamageCauser)
{
  HandleDamage (DamageAmount);
  CombatTarget = EventInstigator->GetPawn ();

  if (IsInsideAttackRadius ())
    {
      EnemyState = EEnemyState::EES_Attacking;
    }
  else if (IsOutsideAttackRadius ())
    {
      ChaseTarget ();
    }

  return DamageAmount;
}

void
AEnemy::Destroyed ()
{
  Super::Destroyed ();

  if (EquippedWeapon)
    {
      EquippedWeapon->Destroy ();
    }
}

void
AEnemy::GetHit_Implementation (const FVector &ImpactPoint, AActor *Hitter)
{
  Super::GetHit_Implementation (ImpactPoint, Hitter);

  if (!IsDead ())
    {
      ShowHealthBar ();
    }

  ClearPatrolTimer ();
  ClearAttackTimer ();
  SetWeaponCollisionEnabled (ECollisionEnabled::NoCollision);

  StopAttackMontage ();
}

void
AEnemy::BeginPlay ()
{
  Super::BeginPlay ();

  EnemyController = Cast<AAIController> (GetController ());

  if (PawnSensing)
    {
      PawnSensing->OnSeePawn.AddDynamic (this, &AEnemy::PawnSeen);
    }

  InitialiseEnemy ();

  Tags.Add (FName ("Enemy"));
}

void
AEnemy::SpawnSoul ()
{
  UWorld *World = GetWorld ();

  if (World && SoulClass && Attributes)
    {
      ASoul *SpawnedSoul = World->SpawnActor<ASoul> (
          SoulClass, GetActorLocation (), GetActorRotation ());
      if (SpawnedSoul)
        {
          SpawnedSoul->SetSouls (Attributes->GetSouls ());
        }
    }
}

void
AEnemy::Die ()
{
  Super::Die ();

  EnemyState = EEnemyState::EES_Dead;
  ClearAttackTimer ();
  DisableCapsule ();
  SetLifeSpan (DeathLifeSpan);
  HideHealthBar ();
  GetCharacterMovement ()->bOrientRotationToMovement = false;
  SetWeaponCollisionEnabled (ECollisionEnabled::NoCollision);
  SpawnSoul ();
}

void
AEnemy::Attack ()
{
  Super::Attack ();

  if (CombatTarget == nullptr)
    {
      return;
    }

  EnemyState = EEnemyState::EES_Engaged;
  PlayAttackMontage ();
}

bool
AEnemy::CanAttack () const
{
  return IsInsideAttackRadius () && !IsAttacking () && !IsDead () && !
         IsEngaged ();
}

void
AEnemy::AttackEnd ()
{
  Super::AttackEnd ();

  EnemyState = EEnemyState::EES_NoState;
  CheckCombatTarget ();
}

void
AEnemy::HandleDamage (float DamageAmount)
{
  Super::HandleDamage (DamageAmount);

  if (Attributes && HealthBarWidget)
    {
      HealthBarWidget->SetHealthPercent (Attributes->GetHealthPercent ());
    }
}

void
AEnemy::InitialiseEnemy ()
{
  HideHealthBar ();
  MoveToTarget (PatrolTarget);
  SpawnDefaultWeapon ();
}

void
AEnemy::CheckPatrolTarget ()
{
  if (InTargetRange (PatrolTarget, PatrolRadius))
    {
      PatrolTarget = ChoosePatrolTarget ();

      const float WaitTime = FMath::RandRange (PatrolWaitMin, PatrolWaitMax);

      GetWorldTimerManager ().SetTimer (
          PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
    }
}

void
AEnemy::CheckCombatTarget ()
{
  if (IsOutsideCombatRadius ())
    {
      ClearAttackTimer ();
      LoseInterest ();

      if (!IsEngaged ())
        {
          StartPatrolling ();
        }
    }
  else if (IsOutsideAttackRadius () && !IsChasing ())
    {
      ClearAttackTimer ();
      if (!IsEngaged ())
        {
          ChaseTarget ();
        }
    }
  else if (CanAttack ())
    {
      StartAttackTimer (); // timer is reset
    }
}

void
AEnemy::PatrolTimerFinished ()
{
  MoveToTarget (PatrolTarget);
}

void
AEnemy::HideHealthBar ()
{
  if (HealthBarWidget)
    {
      HealthBarWidget->SetVisibility (false);
    }
}

void
AEnemy::ShowHealthBar ()
{
  if (HealthBarWidget)
    {
      HealthBarWidget->SetVisibility (true);
    }
}

void
AEnemy::LoseInterest ()
{
  CombatTarget = nullptr;
  HideHealthBar ();
}

void
AEnemy::StartPatrolling ()
{
  EnemyState = EEnemyState::EES_Patrolling;
  GetCharacterMovement ()->MaxWalkSpeed = PatrollingSpeed;
  MoveToTarget (PatrolTarget);
}

void
AEnemy::ChaseTarget ()
{
  EnemyState = EEnemyState::EES_Chasing;
  GetCharacterMovement ()->MaxWalkSpeed = ChasingSpeed;
  MoveToTarget (CombatTarget);
}

bool
AEnemy::IsOutsideCombatRadius () const
{
  return !InTargetRange (CombatTarget, CombatRadius);
}

bool
AEnemy::IsOutsideAttackRadius () const
{
  return !InTargetRange (CombatTarget, AttackRadius);
}

bool
AEnemy::IsInsideAttackRadius () const
{
  return InTargetRange (CombatTarget, AttackRadius);
}

bool
AEnemy::IsChasing () const
{
  return EnemyState == EEnemyState::EES_Chasing;
}

bool
AEnemy::IsAttacking () const
{
  return EnemyState == EEnemyState::EES_Attacking;
}

bool
AEnemy::IsDead () const
{
  return EnemyState == EEnemyState::EES_Dead;
}

bool
AEnemy::IsEngaged () const
{
  return EnemyState == EEnemyState::EES_Engaged;
}

bool
AEnemy::InTargetRange (AActor *Target, double Radius) const
{
  if (!Target)
    {
      return false;
    }

  const double DistanceToTarget
      = (Target->GetActorLocation () - GetActorLocation ()).Size ();
  return DistanceToTarget <= Radius;
}

void
AEnemy::ClearPatrolTimer ()
{
  GetWorldTimerManager ().ClearTimer (PatrolTimer);
}

void
AEnemy::StartAttackTimer ()
{
  EnemyState = EEnemyState::EES_Attacking;
  const float AttackTime = FMath::RandRange (AttackMin, AttackMax);
  GetWorldTimerManager ().SetTimer (AttackTimer, this, &AEnemy::Attack,
                                    AttackTime);
}

void
AEnemy::ClearAttackTimer ()
{
  GetWorldTimerManager ().ClearTimer (AttackTimer);
}

void
AEnemy::MoveToTarget (const AActor *Target) const
{
  if (!EnemyController || !Target)
    {
      return;
    }

  FAIMoveRequest MoveRequest;
  MoveRequest.SetGoalActor (Target);
  MoveRequest.SetAcceptanceRadius (50.f);

  EnemyController->MoveTo (MoveRequest);
}

AActor *
AEnemy::ChoosePatrolTarget ()
{
  AActor *NewPatrolTarget = nullptr;

  TArray<AActor *> AvailableTargets;
  for (auto Target : PatrolTargets)
    {
      if (Target != PatrolTarget)
        {
          AvailableTargets.AddUnique (Target);
        }
    }

  const int32 TotalTargets = AvailableTargets.Num ();

  if (TotalTargets > 0)
    {
      const int32 TargetSelection = FMath::RandRange (0, TotalTargets - 1);
      NewPatrolTarget = AvailableTargets[TargetSelection];
    }

  return NewPatrolTarget;
}

void
AEnemy::SpawnDefaultWeapon ()
{
  UWorld *World = GetWorld ();
  if (World && WeaponClass)
    {
      AWeapon *DefaultWeapon = GetWorld ()->SpawnActor<AWeapon> (WeaponClass);
      DefaultWeapon->Equip (GetMesh (), FName ("RightHandSocket"), this, this);
      EquippedWeapon = DefaultWeapon;
    }
}

void
AEnemy::PawnSeen (APawn *SeenPawn)
{
  const bool bShouldChaseTarget =
      !IsDead () &&
      !IsChasing () &&
      EnemyState < EEnemyState::EES_Attacking &&
      SeenPawn->ActorHasTag (FName ("EngageableTarget")) &&
      !SeenPawn->ActorHasTag (FName ("Dead"));

  if (bShouldChaseTarget)
    {
      CombatTarget = SeenPawn;
      ClearPatrolTimer ();
      ChaseTarget ();
    }
}