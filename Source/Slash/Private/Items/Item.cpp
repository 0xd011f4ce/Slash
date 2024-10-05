// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"
#include "DrawDebugHelpers.h"

#include "Components/SphereComponent.h"

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
      this, &AItem::AItem::OnSphereOverlap);
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
  if (GEngine)
    {
      FString OtherActorName = OtherActor->GetName ();
      GEngine->AddOnScreenDebugMessage (0, 30.f, FColor::Red, OtherActorName);
    }
}

void
AItem::Tick (float DeltaTime)
{
  Super::Tick (DeltaTime);

  RunningTime += DeltaTime;
}