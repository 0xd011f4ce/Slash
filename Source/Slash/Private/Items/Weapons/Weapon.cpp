// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapons/Weapon.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void
AWeapon::BeginPlay ()
{
  Super::BeginPlay ();

  WeaponBox->OnComponentBeginOverlap.AddDynamic (this, &AWeapon::OnBoxOverlap);
}

void
AWeapon::OnSphereOverlap (UPrimitiveComponent *OverlappedComponent,
                          AActor *OtherActor, UPrimitiveComponent *OtherComp,
                          int32 OtherBodyIndex, bool bFromSweep,
                          const FHitResult &SweepResult)
{
  Super::OnSphereOverlap (OverlappedComponent, OtherActor, OtherComp,
                          OtherBodyIndex, bFromSweep, SweepResult);
}

void
AWeapon::OnSphereEndOverlap (UPrimitiveComponent *OverlappedComp,
                             AActor *OtherActor,
                             UPrimitiveComponent *OtherComp,
                             int32 OtherBodyIndex)
{
  Super::OnSphereEndOverlap (OverlappedComp, OtherActor, OtherComp,
                             OtherBodyIndex);
}

void
AWeapon::OnBoxOverlap (UPrimitiveComponent *OverlappedComponent,
                       AActor *OtherActor, UPrimitiveComponent *OtherComp,
                       int32 OtherBodyIndex, bool bFromSweep,
                       const FHitResult &SweepResult)
{
  const FVector Start = BoxTraceStart->GetComponentLocation ();
  const FVector End = BoxTraceEnd->GetComponentLocation ();

  TArray<AActor *> ActorsToIgnore;
  ActorsToIgnore.Add (this);

  FHitResult BoxHit;

  UKismetSystemLibrary::BoxTraceSingle (
      this, Start, End, FVector (5.f, 5.f, 5.f),
      BoxTraceStart->GetComponentRotation (), TraceTypeQuery1, false,
      ActorsToIgnore, EDrawDebugTrace::ForDuration, BoxHit, true);
}

AWeapon::AWeapon ()
{
  WeaponBox = CreateDefaultSubobject<UBoxComponent> (TEXT ("Weapon Box"));
  WeaponBox->SetupAttachment (GetRootComponent ());
  WeaponBox->SetCollisionEnabled (ECollisionEnabled::NoCollision);
  WeaponBox->SetCollisionResponseToAllChannels (ECR_Overlap);
  WeaponBox->SetCollisionResponseToChannel (ECC_Pawn, ECR_Ignore);

  BoxTraceStart
      = CreateDefaultSubobject<USceneComponent> (TEXT ("Box Trace Start"));
  BoxTraceStart->SetupAttachment (GetRootComponent ());

  BoxTraceEnd
      = CreateDefaultSubobject<USceneComponent> (TEXT ("Box Trace End"));
  BoxTraceEnd->SetupAttachment (GetRootComponent ());
}

void
AWeapon::AttachMeshToSocket (USceneComponent *InParent, FName InSocketName)
{
  FAttachmentTransformRules TransformRules (EAttachmentRule::SnapToTarget,
                                            true);
  ItemMesh->AttachToComponent (InParent, TransformRules, InSocketName);
}

void
AWeapon::Equip (USceneComponent *InParent, FName InSocketName)
{
  AttachMeshToSocket (InParent, InSocketName);
  ItemState = EItemState::EIS_Equipped;

  if (EquipSound)
    {
      UGameplayStatics::PlaySoundAtLocation (this, EquipSound,
                                             GetActorLocation ());
    }

  if (Sphere)
    {
      Sphere->SetCollisionEnabled (ECollisionEnabled::NoCollision);
    }
}