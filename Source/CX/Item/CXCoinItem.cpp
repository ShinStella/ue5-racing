// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/CXCoinItem.h"
#include "CXCoinItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "PlayerState/CXPlayerState.h"
#include "CX/CXPawn.h"

// Sets default values
ACXCoinItem::ACXCoinItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true; // 기본 false
	NetUpdateFrequency = 1.f; // replication 빈도 설정. 초기값은 100
	MinNetUpdateFrequency = 1.f;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(SceneRoot);
	OverlapSphere->SetSphereRadius(100.0f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	OverlapSphere->SetGenerateOverlapEvents(true);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(SceneRoot);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ACXCoinItem::BeginPlay()
{
	Super::BeginPlay();

	// 서버일 때만 바인딩
	if (HasAuthority())
	{
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ACXCoinItem::OnOverlapSphereBeginOverlap);
	}
}

void ACXCoinItem::OnOverlapSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	ACXPawn* VehiclePawn = Cast<ACXPawn>(OtherActor);
	if (!IsValid(VehiclePawn))
	{
		return;
	}

	if (ACXPlayerState* PlayerState = Cast<ACXPlayerState>(VehiclePawn->GetPlayerState()))
	{
		PlayerState->AddCoins(10);
	}

	MulticastPickupEffects();

	Destroy();
}

void ACXCoinItem::MulticastPickupEffects_Implementation()
{
	UWorld* World = GetWorld();

	if (IsValid(PickupParticle))
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, PickupParticle, GetActorLocation());
	}

	if (IsValid(PickupSound))
	{
		UGameplayStatics::PlaySoundAtLocation(World, PickupSound, GetActorLocation());
	}
}

// Called every frame
void ACXCoinItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

