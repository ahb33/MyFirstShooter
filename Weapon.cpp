// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

AWeapon::AWeapon():
Ammo(0),
WeaponType(EWeaponType::EWT_AssaultRifle),
RifleCapacity(30), // change this later in blueprint
AmmoType(EAmmoType::EAT_AR),
AmmoInMag(30)
{

}

void AWeapon::Tick(float DeltaTime)
{
	
    Super::Tick(DeltaTime);
}

void AWeapon::DecrementAmmo()
{
    if (Ammo -1 <= 0)
    {
        Ammo = 0;
    }

    else
    {
        Ammo--;
    }
}

void AWeapon::loadAmmo(int32 bullets)
{
	Ammo += bullets;
}

bool AWeapon::HasAmmo()
{
    if(Ammo > 0)
    {
        return true;
    }

    else
    {
        return false;
    }
}
/*

15 bullets on hand, rifle capacity is 20, ammo in mag is 30,  mag empty space is 5. 
Reloading should never add more than rifle capacity
Reloading should never add more than AmmoInMag 

 */
void AWeapon::Reload()
{
    int32 MagEmptySpace = RifleCapacity - Ammo; 
    if(MagEmptySpace < AmmoInMag ) 
    {
        Ammo += MagEmptySpace;
        AmmoInMag -= MagEmptySpace;
    }
    else // mag empty space > rifle capacity
    {
        Ammo += AmmoInMag;
        AmmoInMag = 0;
    }
}

void AWeapon::LoadPickedupAmmo()
{
    int32 AmmoCount = ItemCount->GetItemCount(); // get ammount of ammo in scene
    AmmoInMag += AmmoCount;
}