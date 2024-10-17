// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Item.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UNiagaraComponent;

enum class EItemState : uint8
{
  EIS_Hovering,
  EIS_Equipped
};

UCLASS ()
class SLASH_API AItem : public AActor
{
  GENERATED_BODY ()

public:
  AItem ();

  virtual void Tick (float DeltaTime) override;

protected:
  virtual void BeginPlay () override;

  UPROPERTY (VisibleAnywhere, BlueprintReadOnly)
  float RunningTime;

  // EditDefaultsOnly, EditInstanceOnly
  UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
  float Amplitude = 0.25f;

  UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
  float TimeConstant = 5.f;

  UFUNCTION (BlueprintPure)
  float TransformedSine ();

  UFUNCTION (BlueprintPure)
  float TransformedCosine ();

  UFUNCTION ()
  virtual void OnSphereOverlap (UPrimitiveComponent *OverlappedComponent,
                                AActor *OtherActor,
                                UPrimitiveComponent *OtherComp,
                                int32 OtherBodyIndex, bool bFromSweep,
                                const FHitResult &SweepResult);

  UFUNCTION ()
  virtual void OnSphereEndOverlap (UPrimitiveComponent *OverlappedComp,
                                   AActor *OtherActor,
                                   UPrimitiveComponent *OtherComp,
                                   int32 OtherBodyIndex);

  virtual void SpawnPickupSystem ();
  virtual void SpawnPickupSound ();

  UPROPERTY (VisibleAnywhere, BlueprintReadOnly)
  UStaticMeshComponent *ItemMesh;

  EItemState ItemState = EItemState::EIS_Hovering;

  UPROPERTY (VisibleAnywhere)
  USphereComponent *Sphere;

  UPROPERTY (EditAnywhere)
  UNiagaraComponent *ItemEffect;

  UPROPERTY (EditAnywhere)
  UNiagaraSystem *PickupEffect;

  UPROPERTY (EditAnywhere)
  USoundBase *PickupSound;
};