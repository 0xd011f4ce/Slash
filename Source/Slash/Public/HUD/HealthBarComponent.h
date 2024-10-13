// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/WidgetComponent.h"
#include "CoreMinimal.h"

#include "HealthBarComponent.generated.h"

class UHealthBar;

UCLASS ()
class SLASH_API UHealthBarComponent : public UWidgetComponent
{
  GENERATED_BODY ()

public:
  void SetHealthPercent (float Percent);

private:
  UPROPERTY ()
  UHealthBar *HealthBarWidget;
};
