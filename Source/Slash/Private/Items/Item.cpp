// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"
#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(Location)                                                 \
  if (GetWorld ())                                                            \
    DrawDebugSphere (GetWorld (), Location, 25.f, 12, FColor::Red, true);

#define DRAW_LINE(Start, End)                                                 \
  if (GetWorld ())                                                            \
    DrawDebugLine (GetWorld (), Start, End, FColor::Red, true, -1.f, 0, 1.f);

AItem::AItem () { PrimaryActorTick.bCanEverTick = true; }

void
AItem::BeginPlay ()
{
  Super::BeginPlay ();

  FVector Location = GetActorLocation ();
  FVector Forward = GetActorForwardVector ();

  DRAW_SPHERE (Location)
  DRAW_LINE (Location, Location + (Forward * 100))
}

void
AItem::Tick (float DeltaTime)
{
  Super::Tick (DeltaTime);
}
