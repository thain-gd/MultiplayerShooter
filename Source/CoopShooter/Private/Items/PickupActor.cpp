#include "Items/PickupActor.h"

#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"

#include "Items/PowerupActor.h"

// Sets default values
APickupActor::APickupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64.0f, SphereComp->GetScaledSphereRadius(), SphereComp->GetScaledSphereRadius());
	DecalComp->SetupAttachment(RootComponent);

	CooldownDuration = 10.0f;  
	 
	 SetReplicates(true);
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_Authority)
		SpawnPowerup();
}

void APickupActor::SpawnPowerup()
{
	if (!PowerupClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerupClass is nullptr in %s. Please update your Blueprint"), *GetName());
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PowerupInstance = GetWorld()->SpawnActor<APowerupActor>(PowerupClass, GetTransform(), SpawnParams);
	PowerupInstance->SetActorLocation(PowerupInstance->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f));
}


void APickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (GetLocalRole() == ROLE_Authority && PowerupInstance)
	{
		PowerupInstance->ActivatePowerup(OtherActor);
		PowerupInstance = nullptr;

		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &APickupActor::SpawnPowerup, CooldownDuration);
	}
}
