// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"
#include "DrawDebugHelpers.h"

#include "Components/SphereComponent.h"
#include "Characters/SlashCharacter.h"

#include "Slash/DebugMacros.h"

AItem::AItem ()
{
  PrimaryActorTick.bCanEverTick = true;

  ItemMesh = CreateDefaultSubobject<UStaticMeshComponent> (
      TEXT ("ItemMeshComponent"));
  SetRootComponent (ItemMesh);

  Sphere = CreateDefaultSubobject<USphereComponent> (TEXT ("Sphere"));
  Sphere->SetupAttachment (GetRootComponent ());
}

void
AItem::BeginPlay ()
{
  Super::BeginPlay ();

  Sphere->OnComponentBeginOverlap.AddDynamic (
      this, &AItem::OnSphereOverlap);
  Sphere->OnComponentEndOverlap.AddDynamic (this, &AItem::OnSphereEndOverlap);
}

float
AItem::TransformedSine ()
{
  return Amplitude * FMath::Sin (RunningTime * TimeConstant);
}

float
AItem::TransformedCosine ()
{
  return Amplitude * FMath::Cos (RunningTime * TimeConstant);
}

void
AItem::OnSphereOverlap (UPrimitiveComponent *OverlappedComponent,
                        AActor *OtherActor, UPrimitiveComponent *OtherComp,
                        int32 OtherBodyIndex,
                        bool bFromSweep, const FHitResult &SweepResult)
{
  if (ASlashCharacter *SlashCharacter = Cast<ASlashCharacter> (OtherActor))
    {
      SlashCharacter->SetOverlappingItem (this);
    }
}

void
AItem::OnSphereEndOverlap (UPrimitiveComponent *OverlappedComp,
                           AActor *OtherActor, UPrimitiveComponent *OtherComp,
                           int32 OtherBodyIndex)
{
  if (ASlashCharacter *SlashCharacter = Cast<ASlashCharacter> (OtherActor))
    {
      SlashCharacter->SetOverlappingItem (nullptr);
    }
}

void
AItem::Tick (float DeltaTime)
{
  Super::Tick (DeltaTime);

  RunningTime += DeltaTime;

  if (ItemState == EItemState::EIS_Hovering)
    {
      AddActorWorldOffset (FVector (0.f, 0.f, TransformedSine ()));
    }
}