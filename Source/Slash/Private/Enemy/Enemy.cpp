// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"

#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "HUD/HealthBarComponent.h"

#include "Animation/AnimMontage.h"

#include "Slash/DebugMacros.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

AEnemy::AEnemy ()
{
  PrimaryActorTick.bCanEverTick = true;

  GetMesh ()->SetCollisionObjectType (ECollisionChannel::ECC_WorldDynamic);
  GetMesh ()->SetCollisionResponseToChannel (ECollisionChannel::ECC_Visibility,
                                             ECollisionResponse::ECR_Block);
  GetMesh ()->SetCollisionResponseToChannel (ECollisionChannel::ECC_Camera,
                                             ECollisionResponse::ECR_Ignore);
  GetMesh ()->SetGenerateOverlapEvents (true);

  GetCapsuleComponent ()->SetCollisionResponseToChannel (
      ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

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
AEnemy::BeginPlay ()
{
  Super::BeginPlay ();

  if (HealthBarWidget)
    {
      HealthBarWidget->SetVisibility (false);
    }

  EnemyController = Cast<AAIController> (GetController ());
  MoveToTarget (PatrolTarget);

  if (PawnSensing)
    {
      PawnSensing->OnSeePawn.AddDynamic (this, &AEnemy::PawnSeen);
    }
}

void
AEnemy::Die ()
{
  UAnimInstance *AnimInstance = GetMesh ()->GetAnimInstance ();
  if (AnimInstance && DeathMontage)
    {
      AnimInstance->Montage_Play (DeathMontage);

      const int32 Selection = FMath::RandRange (0, 4);
      FName SectionName = FName ();
      switch (Selection)
        {
        case 0:
          SectionName = FName ("Death1");
          DeathPose = EDeathPose::EDP_Death1;
          break;

        case 1:
          SectionName = FName ("Death2");
          DeathPose = EDeathPose::EDP_Death2;
          break;

        case 2:
          SectionName = FName ("Death3");
          DeathPose = EDeathPose::EDP_Death3;
          break;

        case 3:
          SectionName = FName ("Death4");
          DeathPose = EDeathPose::EDP_Death4;
          break;

        case 4:
          SectionName = FName ("Death5");
          DeathPose = EDeathPose::EDP_Death5;
          break;
        }

      AnimInstance->Montage_JumpToSection (SectionName, DeathMontage);
    }

  GetCapsuleComponent ()->SetCollisionEnabled (ECollisionEnabled::NoCollision);
  SetLifeSpan (10.f);

  if (HealthBarWidget)
    {
      HealthBarWidget->SetVisibility (false);
    }
}

bool
AEnemy::InTargetRange (AActor *Target, double Radius)
{
  if (!Target)
    return false;

  const double DistanceToTarget
      = (Target->GetActorLocation () - GetActorLocation ()).Size ();
  return DistanceToTarget <= Radius;
}

void
AEnemy::MoveToTarget (AActor *Target)
{
  if (!EnemyController || !Target)
    return;

  FAIMoveRequest MoveRequest;
  MoveRequest.SetGoalActor (Target);
  MoveRequest.SetAcceptanceRadius (15.f);

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
AEnemy::PawnSeen (APawn *SeenPawn)
{
  if (EnemyState == EEnemyState::EES_Chasing)
    return;

  if (SeenPawn->ActorHasTag (FName ("SlashCharacter")))
    {
      GetWorldTimerManager ().ClearTimer (PatrolTimer);
      GetCharacterMovement ()->MaxWalkSpeed = 300.f;
      CombatTarget = SeenPawn;

      if (EnemyState != EEnemyState::EES_Attacking)
        {
          EnemyState = EEnemyState::EES_Chasing;
          MoveToTarget (CombatTarget);
        }
    }
}

void
AEnemy::PatrolTimerFinished ()
{
  MoveToTarget (PatrolTarget);
}

void
AEnemy::Tick (float DeltaTime)
{
  Super::Tick (DeltaTime);

  if (EnemyState > EEnemyState::EES_Patrolling)
    {
      CheckCombatTarget ();
    }
  else
    {
      CheckPatrolTarget ();
    }
}

void
AEnemy::CheckPatrolTarget ()
{
  if (InTargetRange (PatrolTarget, PatrolRadius))
    {
      PatrolTarget = ChoosePatrolTarget ();

      const float WaitTime = FMath::RandRange (WaitMin, WaitMax);

      GetWorldTimerManager ().SetTimer (
          PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
    }
}

void
AEnemy::CheckCombatTarget ()
{
  if (!InTargetRange (CombatTarget, CombatRadius))
    {
      // outside combat radius, lose interest
      CombatTarget = nullptr;

      if (HealthBarWidget)
        {
          HealthBarWidget->SetVisibility (false);
        }

      EnemyState = EEnemyState::EES_Patrolling;
      GetCharacterMovement ()->MaxWalkSpeed = 150.f;
      MoveToTarget (PatrolTarget);
    }
  else if (!InTargetRange (CombatTarget, AttackRadius)
           && EnemyState != EEnemyState::EES_Chasing)
    {
      // outside attack range, chase character
      EnemyState = EEnemyState::EES_Chasing;
      GetCharacterMovement ()->MaxWalkSpeed = 300.f;
      MoveToTarget (CombatTarget);
    }
  else if (InTargetRange (CombatTarget, AttackRadius)
           && EnemyState != EEnemyState::EES_Attacking)
    {
      // inside attack range, attack character
      EnemyState = EEnemyState::EES_Attacking;

      // TODO: Attack montage
    }
}

void
AEnemy::SetupPlayerInputComponent (UInputComponent *PlayerInputComponent)
{
  Super::SetupPlayerInputComponent (PlayerInputComponent);
}

void
AEnemy::GetHit_Implementation (const FVector &ImpactPoint)
{
  if (HealthBarWidget)
    {
      HealthBarWidget->SetVisibility (true);
    }

  if (Attributes && Attributes->IsAlive ())
    {
      DirectionalHitReact (ImpactPoint);
    }
  else
    {
      Die ();
    }

  if (HitSound)
    {
      UGameplayStatics::PlaySoundAtLocation (this, HitSound, ImpactPoint);
    }

  if (HitParticles && GetWorld ())
    {
      UGameplayStatics::SpawnEmitterAtLocation (GetWorld (), HitParticles,
                                                ImpactPoint);
    }
}

float
AEnemy::TakeDamage (float DamageAmount, FDamageEvent const &DamageEvent,
                    AController *EventInstigator, AActor *DamageCauser)
{
  if (Attributes && HealthBarWidget)
    {
      Attributes->ReceiveDamage (DamageAmount);
      HealthBarWidget->SetHealthPercent (Attributes->GetHealthPercent ());
    }

  CombatTarget = EventInstigator->GetPawn ();
  EnemyState = EEnemyState::EES_Chasing;
  GetCharacterMovement ()->MaxWalkSpeed = 300.f;
  MoveToTarget (CombatTarget);

  return DamageAmount;
}