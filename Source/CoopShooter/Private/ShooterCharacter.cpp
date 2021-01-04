#include "ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Actors/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	CharacterMovementComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharacterMovementComp)
	{
		CharacterMovementComp->GetNavAgentPropertiesRef().bCanCrouch = true;
		DefaultMovingSpeed = CharacterMovementComp->MaxWalkSpeed;
		AimingMovingSpeed = DefaultMovingSpeed * 0.35f;
	}

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	AimingFOV = 65.0f;
	AimInterpSpeed = 20.0f;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = CameraComp->FieldOfView;
	WeaponAttachSocketName = TEXT("weapon_socket");

	SetupWeapons();
}

void AShooterCharacter::SetupWeapons()
{
	int i = 0;
	for (TSubclassOf<AWeapon> WeaponClass : WeaponClasses)
	{
		if (WeaponClass)
		{
			AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponAttachSocketName);
			Weapon->SetOwner(this);

			if (i != CurrentWeaponIndex)
			{
				Weapon->SetActorHiddenInGame(true);
			}

			AvailableWeapons.Add(Weapon);
		}

		++i;
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCameraFOV(DeltaTime);
}

void AShooterCharacter::UpdateCameraFOV(float DeltaTime)
{
	float TargetFOV = bIsAiming ? AimingFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, AimInterpSpeed);
	CameraComp->SetFieldOfView(NewFOV);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &AShooterCharacter::BeginCrouch);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &AShooterCharacter::EndCrouch);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AShooterCharacter::StartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &AShooterCharacter::StopFire);

	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Pressed, this, &AShooterCharacter::BeginAim);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Released, this, &AShooterCharacter::EndAim);

	PlayerInputComponent->BindAction(TEXT("SwitchWeaponUp"), IE_Pressed, this, &AShooterCharacter::SwitchWeaponUp);
	PlayerInputComponent->BindAction(TEXT("SwitchWeaponDown"), IE_Pressed, this, &AShooterCharacter::SwitchWeaponDown);
}

void AShooterCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AShooterCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AShooterCharacter::BeginCrouch()
{
	Crouch();
}

void AShooterCharacter::EndCrouch()
{
	UnCrouch();
}

void AShooterCharacter::StartFire()
{
	if (AvailableWeapons.Num() - 1 >= CurrentWeaponIndex)
		AvailableWeapons[CurrentWeaponIndex]->StartFire();
}

void AShooterCharacter::StopFire()
{
	if (AvailableWeapons.Num() - 1 >= CurrentWeaponIndex)
		AvailableWeapons[CurrentWeaponIndex]->StopFire();
}

void AShooterCharacter::BeginAim()
{
	bIsAiming = true;
	if (CharacterMovementComp)
		CharacterMovementComp->MaxWalkSpeed = AimingMovingSpeed;
}

void AShooterCharacter::EndAim()
{
	bIsAiming = false;
	if (CharacterMovementComp)
		CharacterMovementComp->MaxWalkSpeed = DefaultMovingSpeed;
}

void AShooterCharacter::SwitchWeaponUp()
{
	SwitchWeapon((CurrentWeaponIndex - 1 + AvailableWeapons.Num()) % AvailableWeapons.Num());
}

void AShooterCharacter::SwitchWeaponDown()
{
	SwitchWeapon((CurrentWeaponIndex + 1) % AvailableWeapons.Num());
}

void AShooterCharacter::SwitchWeapon(uint32 NextWeaponIndex)
{
	AvailableWeapons[CurrentWeaponIndex]->SetActorHiddenInGame(true);
	AvailableWeapons[NextWeaponIndex]->SetActorHiddenInGame(false);
	CurrentWeaponIndex = NextWeaponIndex;
}

FVector AShooterCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}
