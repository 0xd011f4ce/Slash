// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapons/Weapon.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "NiagaraComponent.h"

#include "Interfaces/HitInterface.h"

void
AWeapon::BeginPlay ()
{
  Super::BeginPlay ();

  WeaponBox->OnComponentBeginOverlap.AddDynamic (this, &AWeapon::OnBoxOverlap);
}

void
AWeapon::ExecuteGetHit (FHitResult BoxHit)
{
  IHitInterface *HitInterface = Cast<IHitInterface> (BoxHit.GetActor ());
  if (HitInterface)
    {
      HitInterface->Execute_GetHit (BoxHit.GetActor (),
                                    BoxHit.ImpactPoint, GetOwner ());
    }
}

bool
AWeapon::ActorIsSameType (AActor *OtherActor)
{
  return GetOwner ()->ActorHasTag (TEXT ("Enemy")) && OtherActor->ActorHasTag (
             TEXT ("Enemy"));
}

void
AWeapon::OnBoxOverlap (UPrimitiveComponent *OverlappedComponent,
                       AActor *OtherActor, UPrimitiveComponent *OtherComp,
                       int32 OtherBodyIndex, bool bFromSweep,
                       const FHitResult &SweepResult)
{
  if (ActorIsSameType (OtherActor))
    {
      return;
    }

  FHitResult BoxHit;
  BoxTrace (BoxHit);

  if (BoxHit.GetActor ())
    {
      if (ActorIsSameType (BoxHit.GetActor ()))
        {
          return;
        }

      UGameplayStatics::ApplyDamage (BoxHit.GetActor (), Damage,
                                     GetInstigator ()->GetController (), this,
                                     UDamageType::StaticClass ());
      ExecuteGetHit (BoxHit);
      CreateFields (BoxHit.ImpactPoint);
    }
}

AWeapon::AWeapon ()
{
  WeaponBox = CreateDefaultSubobject<UBoxComponent> (TEXT ("Weapon Box"));
  WeaponBox->SetupAttachment (GetRootComponent ());
  WeaponBox->SetCollisionEnabled (ECollisionEnabled::NoCollision);
  WeaponBox->SetCollisionResponseToAllChannels (ECR_Overlap);
  WeaponBox->SetCollisionResponseToChannel (ECC_Pawn, ECR_Ignore);

  BoxTraceStart
      = CreateDefaultSubobject<USceneComponent> (TEXT ("Box Trace Start"));
  BoxTraceStart->SetupAttachment (GetRootComponent ());

  BoxTraceEnd
      = CreateDefaultSubobject<USceneComponent> (TEXT ("Box Trace End"));
  BoxTraceEnd->SetupAttachment (GetRootComponent ());
}

void
AWeapon::AttachMeshToSocket (USceneComponent *InParent, FName InSocketName)
{
  FAttachmentTransformRules TransformRules (EAttachmentRule::SnapToTarget,
                                            true);
  ItemMesh->AttachToComponent (InParent, TransformRules, InSocketName);
}

void
AWeapon::PlayEquipSound ()
{
  if (EquipSound)
    {
      UGameplayStatics::PlaySoundAtLocation (this, EquipSound,
                                             GetActorLocation ());
    }
}

void
AWeapon::DisableSphereCollision ()
{
  if (Sphere)
    {
      Sphere->SetCollisionEnabled (ECollisionEnabled::NoCollision);
    }
}

void
AWeapon::DeactivateEmbers ()
{
  if (EmbersEffect)
    {
      EmbersEffect->Deactivate ();
    }
}

void
AWeapon::BoxTrace (FHitResult &BoxHit)
{
  const FVector Start = BoxTraceStart->GetComponentLocation ();
  const FVector End = BoxTraceEnd->GetComponentLocation ();

  TArray<AActor *> ActorsToIgnore;
  ActorsToIgnore.Add (this);

  for (AActor *Actor : IgnoreActors)
    {
      ActorsToIgnore.Add (Actor);
    }

  UKismetSystemLibrary::BoxTraceSingle (
      this, Start, End, BoxTraceExtent, BoxTraceStart->GetComponentRotation (),
      TraceTypeQuery1, false,
      ActorsToIgnore,
      bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
      BoxHit, true);

  IgnoreActors.AddUnique (BoxHit.GetActor ());
}

void
AWeapon::Equip (USceneComponent *InParent, FName InSocketName,
                AActor *NewOwner, APawn *NewInstigator)
{
  ItemState = EItemState::EIS_Equipped;

  SetOwner (NewOwner);
  SetInstigator (NewInstigator);
  AttachMeshToSocket (InParent, InSocketName);
  DisableSphereCollision ();

  PlayEquipSound ();
  DeactivateEmbers ();
}