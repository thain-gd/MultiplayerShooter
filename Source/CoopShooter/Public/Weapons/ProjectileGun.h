// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "ProjectileGun.generated.h"

//class AProjectile;

UCLASS()
class COOPSHOOTER_API AProjectileGun : public AWeapon
{
	GENERATED_BODY()
	
public:
	virtual void OnFireHandle(AActor* MyOwner) override;
	
	float GetDamageRadius() const { return DamageRadius; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile Gun")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Projectile Gun")
	float DamageRadius;
};
