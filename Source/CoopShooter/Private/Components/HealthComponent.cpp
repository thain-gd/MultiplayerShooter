#include "Components/HealthComponent.h"
#include "WaveBasedGameMode.h"
#include "Net/UnrealNetwork.h"

UHealthComponent::UHealthComponent()
{
	MaxHealth = 100;

	SetIsReplicated(true);
}

void UHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || CurrentHealth <= 0.0f)
		return;

	CurrentHealth = FMath::Min(CurrentHealth + HealAmount, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(CurrentHealth), *FString::SanitizeFloat(HealAmount));

	OnHealthChanged.Broadcast(this, CurrentHealth, -HealAmount, nullptr, nullptr, nullptr);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);

		CurrentHealth = MaxHealth;
	}
}

void UHealthComponent::HandleTakeAnyDamage(AActor* OnTakeAnyDamage, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || !IsAlive())
		return;

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);

	if (IsAlive())
		return;

	AWaveBasedGameMode* GM = GetWorld()->GetAuthGameMode<AWaveBasedGameMode>();
	if (GM)
		GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
}

void UHealthComponent::OnRep_CurrentHealth(float PreviousHealth)
{
	OnHealthChanged.Broadcast(this, CurrentHealth, PreviousHealth - CurrentHealth, nullptr, nullptr, nullptr);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}

