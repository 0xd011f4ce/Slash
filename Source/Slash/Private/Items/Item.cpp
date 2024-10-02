// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"
#include "DrawDebugHelpers.h"

#include "Slash/DebugMacros.h"

AItem::AItem () { PrimaryActorTick.bCanEverTick = true; }

void
AItem::BeginPlay ()
{
  Super::BeginPlay ();
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
AItem::Tick (float DeltaTime)
{
  Super::Tick (DeltaTime);

  RunningTime += DeltaTime;

  // float DeltaZ = Amplitude * FMath::Sin (RunningTime * TimeConstant);
  // AddActorWorldOffset (FVector (0.f, 0.f, DeltaZ));

  FVector Location = GetActorLocation ();
  FVector Forward = GetActorForwardVector ();

  DRAW_SPHERE_SingleFrame (Location);
  DRAW_VECTOR_SingleFrame (Location, Location + (Forward * 100));
}
