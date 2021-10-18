// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items.h"
#include "ShooterCharacter.h"
#include "Ammo.h"
#include "AmmoType.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle")
};


UCLASS()
class MYFIRSTSHOOTER_API AWeapon : public AItems
{
	GENERATED_BODY()
public:
	AWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void loadAmmo(int32 Bullets);
	
	void Reload();

	void LoadPickedupAmmo();

	class AItems* ItemCount;


private: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	int32 Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	int32 RifleCapacity; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	int32 AmmoInMag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	int32 LoadedAmmo;

public:

	FORCEINLINE int32 GetAmmo() const {return Ammo;}
	FORCEINLINE int32 Get_RifleCapacity() const {return RifleCapacity;}

	void DecrementAmmo();

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType;}
	FORCEINLINE EAmmoType GetAmmoType() const {return AmmoType; }

	FORCEINLINE int32 GetAmmoInMag() const {return AmmoInMag;}

	bool HasAmmo(); // this function will check for ammo


};
