// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveGameState.h"
#include "Net/UnrealNetwork.h"

void AWaveGameState::SetWaveState(EWaveState NewState)
{
	EWaveState OldState = WaveState;
	WaveState = NewState;
	OnRep_WaveState(OldState);
}

void AWaveGameState::OnRep_WaveState(EWaveState OldState)
{
	WaveStateChanged(WaveState, OldState);
}

void AWaveGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWaveGameState, WaveState);
}
