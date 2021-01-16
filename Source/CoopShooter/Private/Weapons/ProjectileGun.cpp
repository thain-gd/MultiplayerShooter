// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileGun.h"
#include "Weapons/Projectile.h"
#include <Engine/World.h>

void AProjectileGun::OnFireHandle(AActor* MyOwner)
{
	if (ProjectileClass)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		
		if (GetLocalRole() == ROLE_Authority)
		{
			auto projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, EyeRotation);
			projectile->Initialize(BaseDamage, DamageRadius);
		}
		
		LastTimeFire = GetWorld()->TimeSeconds;
	}
}
