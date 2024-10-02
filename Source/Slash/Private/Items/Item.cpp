// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"

AItem::AItem () { PrimaryActorTick.bCanEverTick = true; }

void
AItem::BeginPlay ()
{
  Super::BeginPlay ();

  if (GEngine)
    {
      GEngine->AddOnScreenDebugMessage (1, 60.f, FColor::Cyan,
                                        TEXT ("We are using item!"));
    }
}

void
AItem::Tick (float DeltaTime)
{
  Super::Tick (DeltaTime);

  UE_LOG (LogTemp, Warning, TEXT ("DeltaTime:  %f"), DeltaTime)

  if (GEngine)
    {
      FString Name = GetName ();
      FString Message = FString::Printf (TEXT ("Name: %s\nDeltaTime: %f"),
                                         *Name, DeltaTime);
      GEngine->AddOnScreenDebugMessage (1, 60.f, FColor::Red, Message);
    }
}
