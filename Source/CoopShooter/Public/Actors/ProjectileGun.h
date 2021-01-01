// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapon.h"
#include "ProjectileGun.generated.h"

//class AProjectile;

UCLASS()
class COOPSHOOTER_API AProjectileGun : public AWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire() override;
	
	float GetDamageRadius() const { return DamageRadius; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile Gun")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Projectile Gun")
	float DamageRadius;
};
