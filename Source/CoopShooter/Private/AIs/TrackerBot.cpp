// Fill out your copyright notice in the Description page of Project Settings.


#include "AIs/TrackerBot.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/HealthComponent.h"
#include "Components/AudioComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "ShooterCharacter.h"
#include "Sound/SoundCue.h"

#define I_AM_AUTHORITY GetLocalRole() == ROLE_Authority

// Sets default values
ATrackerBot::ATrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(200);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetupAttachment(RootComponent);

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));

	bUseVelocityChange = false;
	MovementForce = 1000;
	RequiredDistanceToTarget = 100;

	ExplosionDamage = 100;
	ExplosionRadius = 200;
	SelfDamageInterval = 0.3f;
}

// Called when the game starts or when spawned
void ATrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
	HealthComp->OnHealthChanged.AddDynamic(this, &ATrackerBot::HandleTakeDamage);
	
	if (I_AM_AUTHORITY)
		NextPathPoint = GetNextPathPoint();
}

void ATrackerBot::HandleTakeDamage(UHealthComponent* HealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!PulseOnDamageMatInstance)
		PulseOnDamageMatInstance = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));

	if (PulseOnDamageMatInstance)
		PulseOnDamageMatInstance->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);

	if (Health == 0)
		SelfDestruct();
}

void ATrackerBot::SelfDestruct()
{
	 if (bExploded)
		return;

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());

	MeshComp->SetVisibility(false, true);
	MeshComp->SetSimulatePhysics(false);

	if (I_AM_AUTHORITY)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		SetLifeSpan(2.0f);
	}
}

FVector ATrackerBot::GetNextPathPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);
	if (NavPath->PathPoints.Num() > 1)
		return NavPath->PathPoints[1];

	return GetActorLocation();
}

// Called every frame
void ATrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() != ROLE_Authority || bExploded)
		return;

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();
	if (DistanceToTarget <= RequiredDistanceToTarget)
	{
		NextPathPoint = GetNextPathPoint();
	}
	else
	{
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();
		ForceDirection *= MovementForce;

		MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
	}
}

void ATrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (bStartedSelfDestruction)
		return;

	AShooterCharacter* PlayerPawn = Cast<AShooterCharacter>(OtherActor);
	if (PlayerPawn)
	{
		if (I_AM_AUTHORITY)
			GetWorldTimerManager().SetTimer(SelfDamageTimerHandle, this, &ATrackerBot::DamageSelf, SelfDamageInterval, true, 0.0f);

		bStartedSelfDestruction = true;

		UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
	}
}

void ATrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}
