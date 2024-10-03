// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"
#include "DrawDebugHelpers.h"

#include "Slash/DebugMacros.h"

AItem::AItem ()
{
  PrimaryActorTick.bCanEverTick = true;

  ItemMesh = CreateDefaultSubobject<UStaticMeshComponent> (
      TEXT ("ItemMeshComponent"));
  SetRootComponent (ItemMesh);
}

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
}