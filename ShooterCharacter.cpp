// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "GenericPlatform/GenericPlatform.h" 
#include "Items.h"
#include "Weapon.h"
#include "Engine/World.h" 
#include "Components/SphereComponent.h" // A sphere generally used for simple collision. Bounds are rendered as lines in the editor.
#include "Components/BoxComponent.h" // A box generally used for simple collision. Bounds are rendered as lines in the editor.
#include "Components/WidgetComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	bShouldTraceForItems(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in toards the character if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f; // The camera follows at this istance behind the character
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 40.f);


	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach camera to end of boom
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// 
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	bDead = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // moves character to direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl= 0.2f;

	bIsAiming = false;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay() 
{
	Super::BeginPlay();
	SpawnDefaultWeapon();



}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)

{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireWeapon);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AShooterCharacter::StartAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AShooterCharacter::StopAiming);

}
  
void AShooterCharacter::MoveForward(float Axis)
{
	if(!bDead) { 
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Axis);		
	}
}

void AShooterCharacter::MoveRight(float Axis)
{
		if(!bDead) { 
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Axis);		
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	// calculate elta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame
}

void AShooterCharacter::LookUpAtRate(float AxisValue)
{
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}
// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceItemCount(); // Traces for items

}

void AShooterCharacter::SpawnDefaultWeapon()
{
	// in order to attach a weapon to character mesh we need to add a socket to skeleton
	if(DefaultWeaponClass) // if this is not set in blueprint it will be null
	{
		// Spawn the weapon
		AWeapon* DefaultWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
		// Get Hand socket
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			// Attach the weapon to the hand
			HandSocket->AttachActor(DefaultWeapon, GetMesh());
			
		}
	EquippedWeapon = DefaultWeapon;
	}
}

void AShooterCharacter::FireWeapon()

// Called to bind functionality to input

{
	/*
	create barrelSocket variable of type USkeletalMeshSocket and get the "barrelSocket" created */
	
	if (EquippedWeapon == nullptr) return;

	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	const USkeletalMeshSocket* barrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("barrelSocket");

	if (barrelSocket)
	{
		const FTransform SocketTransform = barrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());	
		
		if(MuzzleFlash)
	 	{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);
		if(bBeamEnd)
		{
			if(ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEnd);
			}

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
			BeamParticles, SocketTransform);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}

	if(EquippedWeapon)
	{
		EquippedWeapon->DecrementAmmo();
	}

}

void AShooterCharacter::StartAiming()
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{

	// Check for crosshair trace hit
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceForItems(CrosshairHitResult, OutBeamLocation);

	if (bCrosshairHit)
	{
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else
	{
		// OutBeamLocation is the End Location for the line trace
	}

	// Perform trace from gun barrel
	// Perform second trace from gun barrel
	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{MuzzleSocketLocation};
	const FVector StartToEnd{OutBeamLocation - WeaponTraceStart};
	const FVector WeaponTraceEnd{OutBeamLocation + StartToEnd * 1.25f }; 
	GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, 
	ECollisionChannel::ECC_Visibility);
	if (WeaponTraceHit.bBlockingHit) 
	{
		OutBeamLocation = WeaponTraceHit.Location;
		return true;
	}	
	return false;
}

void AShooterCharacter::StopAiming()
{
	bIsAiming = false;
}


void AShooterCharacter::FireButtonPressed()
{
	if(EquippedWeapon->GetAmmo() > 0) // this will check if ammo is available
	{
		bFireButtonPressed = true;
	}
}

bool AShooterCharacter::TraceForItems(FHitResult& OutHitResult, FVector& OutHitLocation) // this function will trace through the crosshairs 
{
	/* To perform line trace through crosshair get viewport size
	
	*/
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrosshairLocation(ViewportSize.X/2, ViewportSize.Y/2);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this,0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		const FVector Start{CrosshairWorldPosition} ;
		const FVector End{Start + CrosshairWorldDirection * 50'000.f};
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		if(OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return true;
} 

void AShooterCharacter::IncrementOverlappedItemCount(int32 OverlappedAmount)
{
	if(OverlappedItemCount + OverlappedAmount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}

	else
	{
		OverlappedItemCount += OverlappedAmount;
		bShouldTraceForItems = true;
	}
}

void AShooterCharacter::TraceItemCount() // traces for items
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceForItems(ItemTraceResult, HitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			AItems* HitItem = Cast<AItems>(ItemTraceResult.Actor);
			if(HitItem && HitItem->GetPickupWidget())
			{
				HitItem->GetPickupWidget()->SetVisibility(true);
			}

			if(TraceHitItemLastFrame)
			{
				if(HitItem != TraceHitItemLastFrame)
				{
					// sets widget visibility to false if we didnt trace for an item in the last frame or AItems is null
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				}
			}
			TraceHitItemLastFrame = HitItem;
		}
	}
	else if (TraceHitItemLastFrame)
	{
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
	}
}


bool AShooterCharacter::DoesWeaponHaveAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	auto AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}

	else 
	{
		return false;
	}
}

void AShooterCharacter::Reload()
{
	UE_LOG(LogTemp, Warning, TEXT("Reload function is being called"));

	if (ReloadSound)
	{
		UGameplayStatics::PlaySound2D(this, ReloadSound);
	}
	FName MontageSection(TEXT("Reload AR4"));
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		AnimInstance->Montage_JumpToSection(MontageSection);
	}
	EquippedWeapon->Reload(); // 2 reload functions in program
}

void AShooterCharacter::ReloadButtonPressed()
{
	Reload();
	UE_LOG(LogTemp, Warning, TEXT("Reloadbutton is being pressed"));
	
}



