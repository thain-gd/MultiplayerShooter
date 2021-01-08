// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Actors/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/ProjectileGun.h"
#include "DrawDebugHelpers.h"

// Sets default values
AProjectile::AProjectile()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);	// set up a notification for when this component hits something blocking
	RootComponent = CollisionComp;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(CollisionComp);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 1000.0f;
	ProjectileMovement->MaxSpeed = 1000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;

	InitialLifeSpan = 3.0f;

	SetReplicates(true);
	SetReplicateMovement(true);
}


void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	TArray<AActor*> IgnoredActors;
	TSubclassOf<UDamageType> DamageType;
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MyOwner->GetDefaultImpactEffect(), GetActorLocation());
	//UGameplayStatics::ApplyRadialDamage(GetWorld(), MyOwner->GetDamage(), Hit.ImpactPoint, MyOwner->GetDamageRadius(), DamageType, IgnoredActors, this);
	Destroy();
}
