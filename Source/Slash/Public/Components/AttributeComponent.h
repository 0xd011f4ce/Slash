// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "AttributeComponent.generated.h"

UCLASS (ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SLASH_API UAttributeComponent : public UActorComponent
{
  GENERATED_BODY ()

public:
  UAttributeComponent ();
  virtual void
  TickComponent (float DeltaTime, ELevelTick TickType,
                 FActorComponentTickFunction *ThisTickFunction) override;

protected:
  virtual void BeginPlay () override;

private:
  // Current Health of the Actor
  UPROPERTY (EditAnywhere, Category = "Actor Attributes")
  float Health;

  UPROPERTY (EditAnywhere, Category = "Actor Attributes")
  float MaxHealth;

  UPROPERTY (EditAnywhere, Category = "Actor Attributes")
  float Strength = 20.f;

  UPROPERTY (EditAnywhere, Category = "Actor Attributes")
  int32 Gold;

  UPROPERTY (EditAnywhere, Category = "Actor Attributes")
  int32 Souls;

  /*
   * Getters and setters
   */
public:
  void ReceiveDamage (float Damage);
  float GetHealthPercent ();
  bool IsAlive ();
  void AddGold (int32 GoldAmount);
  void AddSouls (int32 SoulsAmount);

  FORCEINLINE int32
  GetGold () const { return Gold; }

  FORCEINLINE int32
  GetSouls () const { return Souls; }

  FORCEINLINE float
  GetStrength () const { return Strength; }
};