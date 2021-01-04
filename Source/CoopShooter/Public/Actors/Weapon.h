#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UDamageType;
class UParticleSystem;

UCLASS()
class COOPSHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	void StartFire();
	void StopFire();

	const TSubclassOf<UDamageType> GetDamageType() const { return DamageType; }
	UParticleSystem* GetDefaultImpactEffect() const { return DefaultImpactEffect; }
	float GetDamage() const { return BaseDamage; }

protected:
	void RunCameraShake();

private:
	void Fire();
	virtual void OnFireHandle(AActor* MyOwner);
	void PlayFireEffects();


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	bool bIsSingleTap;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float BaseDamage;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float HeadshotMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float FireRate;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> FireCamShake;

	FVector TraceEndPoint;
	FTimerHandle TimeBetweenShotsTimerHandle;
	float LastTimeFire;
};
