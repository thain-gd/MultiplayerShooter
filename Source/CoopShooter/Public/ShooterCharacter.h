#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

class AWeapon;

UCLASS()
class COOPSHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SetupWeapons();

	void UpdateCameraFOV(float DeltaTime);

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void BeginCrouch();
	void EndCrouch();
	void Fire();
	void BeginAim();
	void EndAim();
	void SwitchWeaponUp();
	void SwitchWeaponDown();
	void SwitchWeapon(uint32 NextWeaponIndex);


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName WeaponAttachSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<TSubclassOf<AWeapon>> WeaponClasses;

	UPROPERTY()
	TArray<AWeapon*> AvailableWeapons;

	uint32 CurrentWeaponIndex = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float AimingFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay", meta = (ClampMin = 0.1f, ClampMax = 100.0f))
	float AimInterpSpeed;

	bool bIsAiming;
	float DefaultFOV;

	float DefaultMovingSpeed;
	float AimingMovingSpeed;

	class UCharacterMovementComponent* CharacterMovementComp;
};
