// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"
#include "DrawDebugHelpers.h"

#include "Slash/DebugMacros.h"

AItem::AItem () { PrimaryActorTick.bCanEverTick = true; }

void
AItem::BeginPlay ()
{
  Super::BeginPlay ();

  SetActorLocation (FVector (0.f, 0.f, 200.f));
  SetActorRotation (FRotator (0.f, 45.f, 0.f));

  FVector Location = GetActorLocation ();
  FVector Forward = GetActorForwardVector ();

  DRAW_SPHERE (Location)
  DRAW_VECTOR (Location, Location + (Forward * 100))
}

void
AItem::Tick (float DeltaTime)
{
  Super::Tick (DeltaTime);
}
