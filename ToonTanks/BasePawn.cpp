// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/CapsuleComponent.h"
//#include "Components/StaticMeshComponent.h"
#include "BasePawn.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"
#include "Particles/ParticleSystem.h"

// Sets default values, 탱크와 타워 모두가 가지는 구성 요소
ABasePawn::ABasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 메인 틀
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComp;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(CapsuleComp);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);
}

void ABasePawn::HandleDestruction()
{
	// TODO : Visual/Sound Effects
	if(DeathParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, DeathParticles, GetActorLocation(), GetActorRotation());
	}

	if(DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	if(DeathCameraShakeClass)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(DeathCameraShakeClass);
	}
}

// 탱크와 타워 둘 다 해당 함수를 쓸 수 있는 이유는 언리얼 내에서 각각의 개체에 서로 다른 클래스를 할당했기 때문에 가능
void ABasePawn::RotateTurret(FVector LookAtTarget)
{
	FVector ToTarget = LookAtTarget - TurretMesh->GetComponentLocation(); // 목적지에서 시작점 값 빼기
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f); // Yaw만 회전하도록 설정

	// 회전값 설정, FMath::RInterpTo 함수를 이용하여 포탑의 부드러운 회전 구현
	TurretMesh->SetWorldRotation(FMath::RInterpTo(TurretMesh->GetComponentRotation(), LookAtRotation, UGameplayStatics::GetWorldDeltaSeconds(this), 5.f));
}

void ABasePawn::Fire()
{
	FVector ProjectileSpawnPointLocation = ProjectileSpawnPoint->GetComponentLocation();

	// 발사 지점 디버깅
	//DrawDebugSphere(GetWorld(), ProjectileSpawnPointLocation, 25.f, 12, FColor::Red, false, 3.f); 

	FRotator Rotation = ProjectileSpawnPoint->GetComponentRotation();

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPointLocation, Rotation);
	Projectile->SetOwner(this);
}
