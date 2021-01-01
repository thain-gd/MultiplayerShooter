// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ProjectileGun.h"
#include "Actors/Projectile.h"
#include <Engine/World.h>

void AProjectileGun::Fire()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner && ProjectileClass)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		
		GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, EyeRotation)->SetMyOwner(this);
	}
}
