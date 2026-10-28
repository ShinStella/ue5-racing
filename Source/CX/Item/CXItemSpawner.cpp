// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/CXItemSpawner.h"
#include "Components/BillboardComponent.h"

// Sets default values
ACXItemSpawner::ACXItemSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 서버에서 스폰되는 아이템은 클라이언트에도 스폰되기 때문에 굳이 bReplicates 켤 필요 없음

	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	SetRootComponent(BillboardComponent);
}

// Called when the game starts or when spawned
void ACXItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SpawnItem();
	}
}

void ACXItemSpawner::SpawnItem()
{
	if (!HasAuthority() || !IsValid(ItemClass))
	{
		return;
	}

	UWorld* World = GetWorld();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	// 지연 스폰
	SpawnedItem = World->SpawnActorDeferred<AActor>(ItemClass, GetActorTransform(), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	SpawnedItem->OnDestroyed.AddDynamic(this, &ACXItemSpawner::OnItemDestroyed);
	SpawnedItem->FinishSpawning(GetActorTransform());
}

void ACXItemSpawner::OnItemDestroyed(AActor* DestroyedActor)
{
	if (!HasAuthority() || !IsValid(ItemClass))
	{
		return;
	}

	SpawnedItem = nullptr;

	GetWorldTimerManager().SetTimer(SpawnTimer, this, &ACXItemSpawner::SpawnItem, SpawnInterval, false);
}

// Called every frame
void ACXItemSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

