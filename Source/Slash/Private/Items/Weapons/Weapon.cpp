// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"

#include "Characters/SlashCharacter.h"

void
AWeapon::OnSphereOverlap (UPrimitiveComponent *OverlappedComponent,
                          AActor *OtherActor, UPrimitiveComponent *OtherComp,
                          int32 OtherBodyIndex,
                          bool bFromSweep, const FHitResult &SweepResult)
{
  Super::OnSphereOverlap (OverlappedComponent, OtherActor, OtherComp,
                          OtherBodyIndex, bFromSweep, SweepResult);
}

void
AWeapon::OnSphereEndOverlap (UPrimitiveComponent *OverlappedComp,
                             AActor *OtherActor,
                             UPrimitiveComponent *OtherComp,
                             int32 OtherBodyIndex)
{
  Super::OnSphereEndOverlap (OverlappedComp, OtherActor, OtherComp,
                             OtherBodyIndex);
}

void
AWeapon::AttachMeshToSocket (USceneComponent *InParent, FName InSocketName)
{
  FAttachmentTransformRules TransformRules (EAttachmentRule::SnapToTarget,
                                            true);
  ItemMesh->AttachToComponent (InParent, TransformRules, InSocketName);
}

void
AWeapon::Equip (USceneComponent *InParent, FName InSocketName)
{
  AttachMeshToSocket (InParent, InSocketName);
  ItemState = EItemState::EIS_Equipped;
}