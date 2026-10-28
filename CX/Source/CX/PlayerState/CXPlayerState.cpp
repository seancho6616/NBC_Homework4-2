// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/CXPlayerState.h"
#include "Net/UnrealNetwork.h"

static int32 GMyTestCoin = 0;
static FAutoConsoleVariableRef CVarAddCoinServer(
	TEXT("cheat.addcoin.server"),
	GMyTestCoin,
	TEXT("서버에서 코인을 입력한 수치만큼 증가 시킵니다."),
	FConsoleVariableDelegate::CreateLambda([](IConsoleVariable* CVar)
		{
			UE_LOG(LogTemp, Warning, TEXT("콘솔 변수 변경됨 ! GMyTestCoin 값 : %d"), GMyTestCoin);

			if (GWorld)
			{
				if (GWorld->GetFirstPlayerController())
				{
					GWorld->GetFirstPlayerController()->GetPlayerState<ACXPlayerState>()->CTSAddCoin(GMyTestCoin);
				}
			}
			GMyTestCoin = 0;
		}),
	ECVF_Cheat
);

static FAutoConsoleVariableRef CVarAddCoinClient(
	TEXT("cheat.addcoin.client"),
	GMyTestCoin,
	TEXT("코인을 입력한 수치만큼 증가 시킵니다."),
	FConsoleVariableDelegate::CreateLambda([](IConsoleVariable* CVar)
		{
			UE_LOG(LogTemp, Warning, TEXT("콘솔 변수 변경됨 ! GMyTestCoin 값 : %d"), GMyTestCoin);

			if (GWorld)
			{
				if (GWorld->GetFirstPlayerController())
				{
					GWorld->GetFirstPlayerController()->GetPlayerState<ACXPlayerState>()->Coins += GMyTestCoin;
				}
			}
			GMyTestCoin = 0;
		}),
	ECVF_Cheat
);

void ACXPlayerState::CTSAddCoin_Implementation(const int32& Amount)
{
	AddCoins(Amount);

}

void ACXPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, Coins, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, BonusMaxSpeed, COND_OwnerOnly);
}

void ACXPlayerState::AddCoins(int32 Amount)
{
	if (HasAuthority() == false || Amount <= 0)
	{
		return;
	}
	FString PlayerName = GetPlayerName();
	Coins += Amount;

	UE_LOG(LogTemp, Warning, TEXT("%s - AddCoins : %d(+%d)"), *PlayerName, Coins,Amount);


}

bool ACXPlayerState::TryRemoveCoins(int32 Amount)
{
	if (HasAuthority() == false || Amount <= 0 || Coins <5)
	{
		return false;
	}
	Coins -= Amount;
	return true;
}

void ACXPlayerState::AddBonusMaxSpeed(float Amount)
{
	if (HasAuthority() == false || Amount <= 0.f)
	{
		return;
	}
	BonusMaxSpeed += Amount;
}

FString ACXPlayerState::GetPlayerStateInfo() const
{
	return FString::Printf(TEXT("Coins : %d , BonusMasSpeed: %.2f"), Coins, BonusMaxSpeed);
}
