// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/BaseCharacter.h"

#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"

#include "Items/Weapons/Weapon.h"

ABaseCharacter::ABaseCharacter ()
{
  PrimaryActorTick.bCanEverTick = true;

  // initialise components
  Attributes
      = CreateDefaultSubobject<UAttributeComponent> (TEXT ("Attributes"));
}

void
ABaseCharacter::BeginPlay ()
{
  Super::BeginPlay ();
}

void
ABaseCharacter::Attack (const FInputActionValue &Value)
{
}

void
ABaseCharacter::Die ()
{
}

void
ABaseCharacter::PlayAttackMontage () const
{
}

void
ABaseCharacter::PlayHitReactMontage (const FName &SectionName)
{
  UAnimInstance *AnimInstance = GetMesh ()->GetAnimInstance ();
  if (AnimInstance && HitReactMontage)
    {
      AnimInstance->Montage_Play (HitReactMontage);
      AnimInstance->Montage_JumpToSection (SectionName, HitReactMontage);
    }
}

void
ABaseCharacter::AttackEnd ()
{
}

bool
ABaseCharacter::CanAttack () const
{
  return false;
}

void
ABaseCharacter::Tick (float DeltaTime)
{
  Super::Tick (DeltaTime);
}

void
ABaseCharacter::SetWeaponCollisionEnabled (
    ECollisionEnabled::Type CollisionEnabled)
{
  if (EquippedWeapon && EquippedWeapon->GetWeaponBox ())
    {
      EquippedWeapon->GetWeaponBox ()->SetCollisionEnabled (CollisionEnabled);
      EquippedWeapon->IgnoreActors.Empty ();
    }
}

void
ABaseCharacter::DirectionalHitReact (const FVector &ImpactPoint)
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
