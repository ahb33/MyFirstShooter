// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Items.h"
#include "Weapon.h"
#include "AmmoType.h"
#include "Particles/ParticleSystemComponent.h"
#include "ShooterCharacter.generated.h"



UCLASS()
class MYFIRSTSHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	AShooterCharacter();

private: 

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon,  meta = (AllowPrivateAccess = "true"))
class AWeapon* EquippedWeapon; // this variable will hold the currently equipped weapon

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for forwards/backwards input */
	void MoveForward(float Value);
	
	
	void MoveRight(float Value);

	/** Called via input to turn at a given rate */
	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	// UFUNCTION(BluePrintCallable)
	// bool SetAiming();

	bool bIsAiming;
	bool bAiming; 

	void StartAiming();
	void StopAiming();

	void EquipWeapon(AWeapon* WeaponToEquip);

	void FireWeapon();// this 


	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation); // 

	void FireButtonPressed();
	void FireButtonReleased();

	void SpawnDefaultWeapon();

	void Reload();

	void ReloadButtonPressed();

	bool bDead;


	bool TraceForItems(FHitResult& OutHitResult, FVector& OutHitLocation); // will trace for items through crosshair

	void TraceItemCount(); // will trace for items

	bool DoesWeaponHaveAmmo();
	bool bFireButtonPressed;

	// this is pointer to the class AAmmo will be used to access functions and variables inside the Ammo class

	
	void PickUpItems(class AItems* Item); 	// this is pointer to the class AAmmo will be used to access functions and variables inside the Ammo class


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Camera that follows the character 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Base turn rate, in degrees per second. Other scaling may affect final turn rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final turn rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon,  meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon,  meta = (AllowPrivateAccess = "true"))
	class USoundCue* ReloadSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon,  meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash; // creates a variable called MuzzleFlash that points to the UParticleSystem
	// class 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon,  meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles; // creates a variable called MuzzleFlash that points to the UParticleSystem
	// class

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon,  meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon,  meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon,  meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* AimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon,  meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* ReloadMontage;

	/*To spawn a weapon we need a UClass which is a variable that holds a reference to a
	blueprint */	
	UPROPERTY(EditDefaultsOnly, BluePrintReadOnly, meta = (AllowPrivateAccess = "true"));
	TSubclassOf<AWeapon> DefaultWeaponClass; // this will allow us to set in blueprint which weapon we will

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AItems* LastItemTraced;

	bool bShouldTraceForItems;

	int32 OverlappedItemCount; // keeps track of number of overlapped items.

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AItems* TraceHitItemLastFrame; 

	/* Map to keep track of ammo of different ammo types */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	UPROPERTY(EditAnywhere)
	float RotationRate = 10.f;

public: 

	/** Returns CameraBoom subobject */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; } // its a good idea when creating a function if we dont plan on changing any members of the class to make the function const
	//Returns FollowCamera subobject  
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE int32 GetOverlappedItemCount() const {return OverlappedItemCount;}

	FORCEINLINE bool GetAiming() const {return bIsAiming; }

	void IncrementOverlappedItemCount(int32 OverlappedAmount);

};


