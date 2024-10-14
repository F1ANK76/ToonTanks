// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanksGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Tank.h"
#include "Tower.h"
#include "ToonTanksPlayerController.h"

void AToonTanksGameMode::ActorDied(AActor *DeadActor)
{
    if(DeadActor == Tank)
    {
        Tank->HandleDestruction();

        // 탱크 제거 처리
        if(ToonTanksPlayerController)
        {
            ToonTanksPlayerController->SetPlayerEnabledState(false);
        }

        // 게임 끝
        GameOver(false);

    }
    // 봇 타워 제거 처리
    else if(ATower* DestroyedTower = Cast<ATower>(DeadActor))
    {
        DestroyedTower->HandleDestruction();

        TargetTowers--;

        if(TargetTowers <= 0)
        {
            // 게임 승리
            GameOver(true);
        }
    }
}

void AToonTanksGameMode::BeginPlay()
{
    Super::BeginPlay();

    HandleGameStart();
}

void AToonTanksGameMode::HandleGameStart()
{
    // 적 포탑 개수 전부 구하기
    TargetTowers = GetTargetTowerCount();

    Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
    ToonTanksPlayerController = Cast<AToonTanksPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

    StartGame();

    // 게임 시작 전, 모든 컨트롤 못하게 막는 부분
    if(ToonTanksPlayerController) // 유효성 검사
    {
        // 탱크 이동 불가
        ToonTanksPlayerController->SetPlayerEnabledState(false);

        FTimerHandle PlayerEnableTimerHandle;
        // 타이머 만료시 해당 함수 재호출 및 인자로 true 전달
        FTimerDelegate PlayerEnableTimerDelegate = FTimerDelegate::CreateUObject(ToonTanksPlayerController, &AToonTanksPlayerController::SetPlayerEnabledState, true);

        // StartDelay 시간동안 탱크 이동 불가 이후 탱크 이동 가능 로직 실행
        GetWorldTimerManager().SetTimer(PlayerEnableTimerHandle, PlayerEnableTimerDelegate, StartDelay, false);
    }
}

int32 AToonTanksGameMode::GetTargetTowerCount()
{
    TArray<AActor*> Towers;
    UGameplayStatics::GetAllActorsOfClass(this, ATower::StaticClass(), Towers);
    return Towers.Num();
}