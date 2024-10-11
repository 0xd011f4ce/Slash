// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Animation/AnimMontage.h"

#include "Slash/DebugMacros.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Enemy/Enemy.h"

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
}

void
AEnemy::BeginPlay ()
{
  Super::BeginPlay ();
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
AEnemy::GetHit (const FVector &ImpactPoint)
{
  DRAW_SPHERE_COLOR (ImpactPoint, FColor::Orange);

  DirectionalHitReact (ImpactPoint);

  if (HitSound)
    {
      UGameplayStatics::PlaySoundAtLocation (this, HitSound, ImpactPoint);
    }
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

  if (GEngine)
    {
      GEngine->AddOnScreenDebugMessage (
          1, 5.f, FColor::Green, FString::Printf (TEXT ("Theta: %f"), Theta));
    }

  UKismetSystemLibrary::DrawDebugArrow (this, GetActorLocation (),
                                        GetActorLocation () + Forward * 60.f,
                                        5.f, FColor::Red, 5.f);
  UKismetSystemLibrary::DrawDebugArrow (this, GetActorLocation (),
                                        GetActorLocation () + ToHit * 60.f,
                                        5.f, FColor::Green, 5.f);
  UKismetSystemLibrary::DrawDebugArrow (
      this, GetActorLocation (), GetActorLocation () + CrossProduct * 100.f,
      5.f, FColor::Blue, 5.f);
}
