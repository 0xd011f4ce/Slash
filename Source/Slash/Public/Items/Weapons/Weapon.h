// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;

UCLASS ()
class SLASH_API AWeapon : public AItem
{
  GENERATED_BODY ()

protected:
  virtual void BeginPlay () override;
  void ExecuteGetHit (FHitResult BoxHit);
  bool ActorIsSameType (AActor *OtherActor);

  UFUNCTION ()
  void OnBoxOverlap (UPrimitiveComponent *OverlappedComponent,
                     AActor *OtherActor, UPrimitiveComponent *OtherComp,
                     int32 OtherBodyIndex, bool bFromSweep,
                     const FHitResult &SweepResult);

  UFUNCTION (BlueprintImplementableEvent)
  void CreateFields (const FVector &FieldLocation);

public:
  AWeapon ();

  void Equip (USceneComponent *InParent, FName InSocketName, AActor *NewOwner,
              APawn *NewInstigator);
  void AttachMeshToSocket (USceneComponent *InParent, FName InSocketName);
  void PlayEquipSound ();
  void DisableSphereCollision ();
  void DeactivateEmbers ();

  TArray<AActor *> IgnoreActors;

private:
  void BoxTrace (FHitResult &BoxHit);

  UPROPERTY (EditAnywhere, Category = "Weapon Properties")
  FVector BoxTraceExtent = FVector (5.f);

  UPROPERTY (EditAnywhere, Category = "Weapon Properties")
  bool bShowBoxDebug = false;

  UPROPERTY (EditAnywhere, Category = "Weapon Properties")
  USoundBase *EquipSound;

  UPROPERTY (VisibleAnywhere, Category = "Weapon Properties")
  UBoxComponent *WeaponBox;

  UPROPERTY (VisibleAnywhere)
  USceneComponent *BoxTraceStart;

  UPROPERTY (VisibleAnywhere)
  USceneComponent *BoxTraceEnd;

  UPROPERTY (EditAnywhere, Category = "Weapon Properties")
  float Damage = 20.f;

public:
  /*
   * Getters and setters
   */
  FORCEINLINE UBoxComponent *
  GetWeaponBox () const
  {
    return WeaponBox;
  }

  FORCEINLINE void
  SetDamage (float Amount) { Damage = Amount; }
};