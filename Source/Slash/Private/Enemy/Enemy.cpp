// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "HUD/HealthBarComponent.h"

#include "Animation/AnimMontage.h"

#include "Slash/DebugMacros.h"

#include "Components/AttributeComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

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

  Attributes
      = CreateDefaultSubobject<UAttributeComponent> (TEXT ("Attributes"));

  HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent> (
      TEXT ("HealthBarWidgetComponent"));
  HealthBarWidget->SetupAttachment (GetRootComponent ());
}

void
AEnemy::BeginPlay ()
{
  Super::BeginPlay ();

  if (HealthBarWidget)
    {
      HealthBarWidget->SetHealthPercent (Attributes->GetHealthPercent ());
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
          break;

        case 1:
          SectionName = FName ("Death2");
          break;

        case 2:
          SectionName = FName ("Death3");
          break;

        case 3:
          SectionName = FName ("Death4");
          break;

        case 4:
          SectionName = FName ("Death5");
          break;
        }

      AnimInstance->Montage_JumpToSection (SectionName, DeathMontage);
    }
}

void
AEnemy::PlayHitReactMontage (const FName &SectionName)
{
  UAnimInstance *AnimInstance = GetMesh ()->GetAnimInstance ();
  if (AnimInstance && HitReactMontage)
    {
      AnimInstance->Montage_Play (HitReactMontage);
      AnimInstance->Montage_JumpToSection (SectionName, HitReactMontage);
    }
}

void
AEnemy::Tick (float DeltaTime)
{
  Super::Tick (DeltaTime);
}

void
AEnemy::SetupPlayerInputComponent (UInputComponent *PlayerInputComponent)
{
  Super::SetupPlayerInputComponent (PlayerInputComponent);
}

void
AEnemy::GetHit_Implementation (const FVector &ImpactPoint)
{
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

  return DamageAmount;
}

void
AEnemy::DirectionalHitReact (const FVector &ImpactPoint)
{
  const FVector Forward = GetActorForwardVector ();
  // Lower the impact point to the same Z as the enemy
  const FVector ImpactLowered (ImpactPoint.X, ImpactPoint.Y,
                               GetActorLocation ().Z);
  const FVector ToHit = (ImpactLowered - GetActorLocation ()).GetSafeNormal ();

  // Forward * ToHit = |Forward| |ToHit| * cos (theta)
  // |Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos (theta)
  const double CosTheta = FVector::DotProduct (Forward, ToHit);
  // Take the inverse cosine of cos(theta) to get theta
  double Theta = FMath::Acos (CosTheta);
  // Convert from radians to degrees
  Theta = FMath::RadiansToDegrees (Theta);

  // If points down, theta should be negative
  const FVector CrossProduct = FVector::CrossProduct (Forward, ToHit);
  if (CrossProduct.Z < 0)
    {
      Theta *= -1.f;
    }

  FName Section ("FromBack");
  if (Theta >= -45.f && Theta < 45.f)
    Section = FName ("FromFront");
  else if (Theta >= -135.f && Theta < -45.f)
    Section = FName ("FromLeft");
  else if (Theta >= 45.f && Theta < 135.f)
    Section = FName ("FromRight");

  PlayHitReactMontage (Section);
}
