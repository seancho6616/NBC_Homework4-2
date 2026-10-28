// CXGameMode.cpp

#include "CXGameMode.h"
#include "CXPlayerController.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Subsystem/ChatSubsystem.h"
#include "PlayerState/CXPlayerState.h"
#include "GameState/CXGameState.h"

ACXGameMode::ACXGameMode()
{
	PlayerControllerClass = ACXPlayerController::StaticClass();
	PlayerStateClass = ACXPlayerState::StaticClass();
	GameStateClass = ACXGameState::StaticClass();

}

void ACXGameMode::BeginPlay()
{
	Super::BeginPlay();

	ACXGameState* CXGameState = GetGameState<ACXGameState>();

	if (IsValid(CXGameState))
	{
		CXGameState->SetGameState(EGameState::Wating);
		CXGameState->SetRemainingTime(5.f);
	}
}



void ACXGameMode::StartWaitingCountdown()
{
	ACXGameState* CXGameState = GetGameState<ACXGameState>();

	bWaingCountdownStarted = true;

	CXGameState->SetGameState(EGameState::Wating);
	CXGameState->SetRemainingTime(5);
	UE_LOG(LogTemp, Log, TEXT("Wating : %d"), CXGameState->GetRemainingTime());
	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &ACXGameMode::HandleWaitingTimerTick, 1.f, true);
}

void ACXGameMode::HandleWaitingTimerTick()
{
	ACXGameState* CXGameState = GetGameState<ACXGameState>();
	const int32 NewRemainingTime = CXGameState->GetRemainingTime() - 1;
	CXGameState->SetRemainingTime(NewRemainingTime);
	if (NewRemainingTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(GameTimerHandle);
		StartPlayingState();
	}
}

void ACXGameMode::StartPlayingState()
{
	ACXGameState* CXGameState = GetGameState<ACXGameState>();
	CXGameState->SetGameState(EGameState::Playing);
	CXGameState->SetRemainingTime(10);
	UE_LOG(LogTemp, Log, TEXT("START : %d"), CXGameState->GetRemainingTime());
	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &ACXGameMode::HandlePlayingTimerTick, 1.f, true);

}

void ACXGameMode::HandlePlayingTimerTick()
{
	ACXGameState* CXGameState = GetGameState<ACXGameState>();
	const int32 NewRemainingTime = CXGameState->GetRemainingTime() - 1;
	CXGameState->SetRemainingTime(NewRemainingTime);
	if (NewRemainingTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(GameTimerHandle);
		FinishGame();
	}
}

void ACXGameMode::FinishGame()
{
	ACXGameState* CXGameState = GetGameState<ACXGameState>();

	UE_LOG(LogTemp, Log, TEXT("Finish"));

	TArray<ACXPlayerState*> Winners;

	int32 HighestCoins = -1;
	for (APlayerState* PlayerState : CXGameState->PlayerArray)
	{
		if (ACXPlayerState* CXPlayerState = Cast<ACXPlayerState>(PlayerState))
		{
			const int32 PlayerCoins = CXPlayerState->GetCoins();
			bool Result = false;
			if (HighestCoins < PlayerCoins)
			{
				HighestCoins = PlayerCoins;
				Winners.Empty();
				Winners.Add(CXPlayerState);
				Result = true;
				
			}
			else if (HighestCoins == PlayerCoins)
			{
				Winners.Add(CXPlayerState);
				Result = true;
			}
			APlayerController* Controller = CXPlayerState->GetPlayerController();
			ACXPlayerController* PlayerController = Cast<ACXPlayerController>(Controller);
			if (!IsValid(PlayerController))
			{
				continue;
			}
			PlayerController->ClientRPCShowGameResult(Result);
		}
	}

	CXGameState->Winners = Winners;
	CXGameState->SetRemainingTime(0);
	CXGameState->SetGameState(EGameState::Finished);
}

AActor* ACXGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<APlayerStart*> AvailablePlayerStarts;

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* PlayerStart = *It;

		if (IsPlayerStartAvailable(PlayerStart))
		{
			AvailablePlayerStarts.Add(PlayerStart);
		}
	}

	checkf(AvailablePlayerStarts.IsEmpty() == false, TEXT("There is no available Vehicle PlayerStart."));

	const int32 RandomIndex = FMath::RandHelper(AvailablePlayerStarts.Num());
	return Cast<AActor>(AvailablePlayerStarts[RandomIndex]);
}

void ACXGameMode::PostLogin(APlayerController* NewPlayer)
{
	if (IsValid(NewPlayer) == false)
	{
		return;
	}

	if (ACXPlayerController* CXPlayerController = Cast<ACXPlayerController>(NewPlayer))
	{
		if (UChatSubsystem* ChatSubSystem = GetWorld()->GetSubsystem<UChatSubsystem>())
		{
			const TArray<FChatData> ChatLog = ChatSubSystem->GetChatLog();
			if(!ChatLog.IsEmpty())
			{
				CXPlayerController->STCChatLog(ChatLog);
			}
		}
	}
	
	Super::PostLogin(NewPlayer);

	if (bWaingCountdownStarted == false)
	{
		StartWaitingCountdown();
	}
}

bool ACXGameMode::IsPlayerStartAvailable(const APlayerStart* PlayerStart) const
{
	if (IsValid(PlayerStart) == false || IsValid(GetWorld()) == false)
	{
		return false;
	}

	const FTransform PlayerStartTransform = PlayerStart->GetActorTransform();
	const FVector CheckLocation = PlayerStartTransform.TransformPosition(VehicleSpawnCheckOffset);
	const FQuat CheckRotation = PlayerStartTransform.GetRotation();
	const FCollisionShape CheckShape = FCollisionShape::MakeBox(VehicleSpawnCheckHalfExtent);
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Vehicle);

	const FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(VehiclePlayerStartCheck), false);

	const bool bVehicleExists =
		GetWorld()->OverlapAnyTestByObjectType(
			CheckLocation,
			CheckRotation,
			ObjectQueryParams,
			CheckShape,
			QueryParams
		);

	return !bVehicleExists;
}