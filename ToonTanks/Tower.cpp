// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"
#include "Tank.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void ATower::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Find the distance to the Tank

    if(InFireRange())
    {
        RotateTurret(Tank->GetActorLocation());
    }
}

void ATower::HandleDestruction()
{
    Super::HandleDestruction();
    Destroy();
}

void ATower::BeginPlay()
{
    Super::BeginPlay();

    Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0)); // 사용자 탱크 정보 호출

    // FireRate 시간마다 CheckFireCondition 함수를 실행
    GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ATower::CheckFireCondition, FireRate, true);
}

void ATower::CheckFireCondition()
{
    if(Tank == nullptr)
    {
        return;
    }

    if(InFireRange() && Tank->bAlive)
    {
        Fire();
    }
}

bool ATower::InFireRange()
{
    if(Tank)
    {
        // 사용자 탱크와 적 타워 사이의 거리 계산
        float Distance = FVector::Dist(GetActorLocation(), Tank->GetActorLocation());

        // 탱크가 사정거리 이내에 있는지 확인
        if(Distance <= FireRange)
        {
            return true;
        }
    }

    return false;
}



