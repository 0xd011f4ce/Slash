// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"
#include "DrawDebugHelpers.h"

#include "Interfaces/PickupInterface.h"
#include "Components/SphereComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "Kismet/GameplayStatics.h"

AItem::AItem ()
{
  PrimaryActorTick.bCanEverTick = true;

  ItemMesh = CreateDefaultSubobject<UStaticMeshComponent> (
      TEXT ("ItemMeshComponent"));
  ItemMesh->SetCollisionResponseToAllChannels (ECR_Ignore);
  ItemMesh->SetCollisionEnabled (ECollisionEnabled::NoCollision);
  SetRootComponent (ItemMesh);

  Sphere = CreateDefaultSubobject<USphereComponent> (TEXT ("Sphere"));
  Sphere->SetupAttachment (GetRootComponent ());

  ItemEffect = CreateDefaultSubobject<UNiagaraComponent> (TEXT ("Embers"));
  ItemEffect->SetupAttachment (GetRootComponent ());
}

void
AItem::BeginPlay ()
{
  Super::BeginPlay ();

  Sphere->OnComponentBeginOverlap.AddDynamic (this, &AItem::OnSphereOverlap);
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
                        int32 OtherBodyIndex, bool bFromSweep,
                        const FHitResult &SweepResult)
{
  if (IPickupInterface *PickupInterface = Cast<IPickupInterface> (OtherActor))
    {
      PickupInterface->SetOverlappingItem (this);
    }
}

void
AItem::OnSphereEndOverlap (UPrimitiveComponent *OverlappedComp,
                           AActor *OtherActor, UPrimitiveComponent *OtherComp,
                           int32 OtherBodyIndex)
{
  if (IPickupInterface *PickupInterface = Cast<IPickupInterface> (OtherActor))
    {
      PickupInterface->SetOverlappingItem (nullptr);
    }
}

void
AItem::SpawnPickupSystem ()
{
  if (PickupEffect)
    {
      UNiagaraFunctionLibrary::SpawnSystemAtLocation (
          this, PickupEffect, GetActorLocation ());
    }
}

void
AItem::SpawnPickupSound ()
{
  if (PickupSound)
    {
      UGameplayStatics::PlaySoundAtLocation (this, PickupSound,
                                             GetActorLocation ());
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