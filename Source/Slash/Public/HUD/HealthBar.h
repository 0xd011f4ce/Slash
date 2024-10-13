// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "HealthBar.generated.h"

class UProgressBar;

UCLASS ()
class SLASH_API UHealthBar : public UUserWidget
{
  GENERATED_BODY ()

public:
  UPROPERTY (meta = (BindWidget))
  UProgressBar *HealthBar;
};
