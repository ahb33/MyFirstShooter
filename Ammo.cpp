// Fill out your copyright notice in the Description page of Project Settings.

#include "Ammo.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"

AAmmo::AAmmo()
{
    // Construct Ammo component and set it as root
    Ammo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ammo"));
    SetRootComponent(Ammo); 

    GetCollisionBox()->SetupAttachment(GetRootComponent()); // attaches the collision box to the root component
    GetPickupWidget()->SetupAttachment(GetRootComponent()); // attaches the widget box to the root component
    GetAreaSphere()->SetupAttachment(GetRootComponent()); // attaches the areasphere box to the root component

    AmmoCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AmmoCollisionSphere"));
    AmmoCollisionSphere->SetupAttachment(GetRootComponent());
    AmmoCollisionSphere->SetSphereRadius(50.f);
}

void AAmmo::Tick(float DeltaTime)
{
	
    Super::Tick(DeltaTime);
}

void AAmmo::BeginPlay()
{
    Super::BeginPlay();
}

void AAmmo::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent*
	OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if(OtherActor->ActorHasTag("Recharge"))
    {
        UE_LOG(LogTemp, Warning, TEXT("Collided with Ammo"));
    }
}

