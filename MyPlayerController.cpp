// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "Blueprint/UserWidget.h"

AMyPlayerController::AMyPlayerController()
{

}

void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (AmmoDisplayClass)
    {
        AmmoDisplay = CreateWidget<UUserWidget>(this, AmmoDisplayClass);
        if (AmmoDisplay)
        {
            AmmoDisplay->AddToViewport();
            AmmoDisplay->SetVisibility(ESlateVisibility::Visible);
        }
    }
}