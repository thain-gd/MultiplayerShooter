#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UDamageType;
class UParticleSystem;

// Contains information of a single hitscan weapon line trace
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

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
	void PlayFireEffects(FVector TraceEndPoint);
	void RunCameraShake();

private:
	void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	virtual void OnFireHandle(AActor* MyOwner);

	void PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	UFUNCTION()
	void OnRep_HitScanTrace();


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

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	FTimerHandle TimeBetweenShotsTimerHandle;
	float LastTimeFire;
};
