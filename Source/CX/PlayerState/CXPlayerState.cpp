// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/CXPlayerState.h"
#include "Net/UnrealNetwork.h"

static int32 GMyTestCoin = 0;
static FAutoConsoleVariableRef CVarAddCoinServer(
	TEXT("cheat.addcoin.server"),
	GMyTestCoin,
	TEXT("add coin request to server"),
	FConsoleVariableDelegate::CreateLambda([](IConsoleVariable* CVar)
		{
			if (CVar)
			{
				UE_LOG(LogTemp, Warning, TEXT("콘솔 변수 변경됨! 현재 값: %d"), CVar->GetInt());
				UE_LOG(LogTemp, Warning, TEXT("콘솔 변수 변경됨! GetMyTestCoin 값: %d"), GMyTestCoin);

				if (GWorld && GWorld->GetFirstPlayerController())
				{
					GWorld->GetFirstPlayerController()->GetPlayerState
						<ACXPlayerState>()->AddCoins(GMyTestCoin);
				}
				GMyTestCoin = 0;
			}
		}),
	ECVF_Cheat
);

static FAutoConsoleVariableRef CVarAddCoinClient(
	TEXT("cheat.addcoin.client"),
	GMyTestCoin,
	TEXT("add coin request to client"),
	FConsoleVariableDelegate::CreateLambda([](IConsoleVariable* CVar)
		{
			UE_LOG(LogTemp, Warning, TEXT("콘솔 변수 변경됨! GetMyTestCoin 값: %d"), GMyTestCoin);

			if (GWorld && GWorld->GetFirstPlayerController())
			{
				GWorld->GetFirstPlayerController()->GetPlayerState
					<ACXPlayerState>()->Coins += GMyTestCoin;
			}
			GMyTestCoin = 0;
		}),
	ECVF_Cheat
);

ACXPlayerState::ACXPlayerState()
{
	// 기본 true긴 하나 연습하고자 적음
	bReplicates = true;
}

void ACXPlayerState::CTSAddCoin_Implementation(const int32& Amount)
{
	// 서버 시점
	AddCoins(Amount);
}

void ACXPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, Coins, COND_OwnerOnly); // Owner만 복제하라
	DOREPLIFETIME_CONDITION(ThisClass, BonusMaxSpeed, COND_OwnerOnly);
}

void ACXPlayerState::AddCoins(int32 Amount)
{
	// 서버일 때만 증감
	if (!HasAuthority() || Amount <= 0)
	{
		return;
	}

	FString PlayerName = GetPlayerName();
	Coins += Amount;
	UE_LOG(LogTemp, Warning, TEXT("%s - AddCoins: %d(+%d)"), *PlayerName, Coins, Amount);
}

bool ACXPlayerState::TryRemoveCoins(int32 Amount)
{
	// 서버일 때만 증감
	if (!HasAuthority() || Amount <= 0 || Coins < 5)
	{
		return false;
	}

	Coins -= Amount;

	return true;
}

void ACXPlayerState::AddBonusMaxSpeed(float Amount)
{
	if (!HasAuthority() || Amount <= 0.f)
	{
		return;
	}

	BonusMaxSpeed += Amount;
}

FString ACXPlayerState::GetPlayerStateInfo() const
{
	return FString::Printf(TEXT("Coins: %d, BonusMaxSpeed: %f"), Coins, BonusMaxSpeed);
}
