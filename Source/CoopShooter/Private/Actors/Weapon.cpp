#include "Actors/Weapon.h"
#include <Kismet/GameplayStatics.h>
#include <Components/SceneComponent.h>
#include <Particles/ParticleSystemComponent.h>
#include <PhysicalMaterials/PhysicalMaterial.h>
#include "../CoopShooter.h"
#include <GameFramework/Actor.h>

// Sets default values
AWeapon::AWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = TEXT("MuzzleFlashSocket");
	TracerTargetName = TEXT("TracerEnd");

	BaseDamage = 20.0f;
}

void AWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		OnFireHandle(MyOwner);

		PlayFireEffects();

		LastTimeFire = GetWorld()->TimeSeconds;
	}
}

void AWeapon::OnFireHandle(AActor* MyOwner)
{
	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector ShotDirection = EyeRotation.Vector();
	FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MyOwner);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	TraceEndPoint = TraceEnd;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
	{
		AActor* HitActor = Hit.GetActor();

		UParticleSystem* SelectedEffect = nullptr;
		EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
		switch (SurfaceType)
		{
		case SURFACE_FLESHDEFAULT:
		case SURFACE_FLESHVULNERABLE:
			SelectedEffect = FleshImpactEffect;
			break;
		default:
			SelectedEffect = DefaultImpactEffect;
			break;
		}

		if (SelectedEffect)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());

		float FinalDamage = BaseDamage;
		if (SurfaceType == SURFACE_FLESHVULNERABLE)
			FinalDamage *= HeadshotMultiplier;

		UGameplayStatics::ApplyPointDamage(HitActor, FinalDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

		TraceEndPoint = Hit.ImpactPoint;
	}
}

void AWeapon::StartFire()
{
	if (!bIsSingleTap)
	{
		float FirstDelay = FMath::Max(LastTimeFire + FireRate - GetWorld()->TimeSeconds, 0.0f);
		GetWorldTimerManager().SetTimer(TimeBetweenShotsTimerHandle, this, &AWeapon::Fire, FireRate, true, FirstDelay);
	}
	else if (GetWorld()->TimeSeconds - LastTimeFire >= FireRate)
	{
		Fire();
	}
}

void AWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimeBetweenShotsTimerHandle);
}

void AWeapon::PlayFireEffects()
{
	if (MuzzleEffect)
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);

	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		auto TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
			TracerComp->SetVectorParameter(TracerTargetName, TraceEndPoint);
	}

	RunCameraShake();
}

void AWeapon::RunCameraShake()
{
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}
}
