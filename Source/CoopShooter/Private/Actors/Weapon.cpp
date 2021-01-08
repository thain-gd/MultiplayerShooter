#include "Actors/Weapon.h"
#include <Kismet/GameplayStatics.h>
#include <Components/SceneComponent.h>
#include <Particles/ParticleSystemComponent.h>
#include <PhysicalMaterials/PhysicalMaterial.h>
#include "../CoopShooter.h"
#include <GameFramework/Actor.h>
#include "Net/UnrealNetwork.h"

// Sets default values
AWeapon::AWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = TEXT("MuzzleFlashSocket");
	TracerTargetName = TEXT("TracerEnd");

	BaseDamage = 20.0f;

	SetReplicates(true);
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

void AWeapon::Fire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		OnFireHandle(MyOwner);
	}
}

void AWeapon::ServerFire_Implementation()
{
	Fire();
}

bool AWeapon::ServerFire_Validate()
{
	return true;
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

	FVector TraceEndPoint = TraceEnd;

	EPhysicalSurface SurfaceType = SurfaceType_Default;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
	{
		AActor* HitActor = Hit.GetActor();

		SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
		PlayImpactEffect(SurfaceType, Hit.ImpactPoint);

		float FinalDamage = BaseDamage;
		if (SurfaceType == SURFACE_FLESHVULNERABLE)
			FinalDamage *= HeadshotMultiplier;

		UGameplayStatics::ApplyPointDamage(HitActor, FinalDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

		TraceEndPoint = Hit.ImpactPoint;
	}

	PlayFireEffects(TraceEndPoint);

	if (GetLocalRole() == ROLE_Authority)
	{
		HitScanTrace.TraceTo = TraceEndPoint;
		HitScanTrace.SurfaceType = SurfaceType;
	}

	LastTimeFire = GetWorld()->TimeSeconds;
}

void AWeapon::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
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
	{
		FVector ShotDirection = ImpactPoint - MeshComp->GetSocketLocation(MuzzleSocketName);
		ShotDirection.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

void AWeapon::PlayFireEffects(FVector TraceEndPoint)
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

void AWeapon::OnRep_HitScanTrace()
{
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffect(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
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

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeapon, HitScanTrace, COND_SkipOwner);
}
