#pragma once

#include "Item/BaseItem.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameInstance/BaseGameInstance.h"
#include "Misc/Region.h"

#include "TGPGameModeBase.generated.h"

class ABaseAiCharacter;
class UAiCharacterData;

USTRUCT(BlueprintType)
struct FPlayerStats
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int EnemiesKilled = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int RoundAchieved = 0;
};

UCLASS()
class TGP_API ATGPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY() TArray<ABaseAiCharacter*> EnemyPool;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning", Meta = (AllowPrivateAccess = true)) FVector SpawnMinRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning", Meta = (AllowPrivateAccess = true)) FVector SpawnMaxRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning", Meta = (AllowPrivateAccess = true)) float MinSpawnDistFromPlayer = 300.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning", Meta = (AllowPrivateAccess = true)) float SpawnIncreaseExponential = 0.15f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning", Meta = (AllowPrivateAccess = true)) float SpawnTimer = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RoundSettings", Meta = (AllowPrivateAccess = true)) float CooldownBetweenRounds = 5.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemies", Meta = (AllowPrivateAccess = true)) TSubclassOf<ABaseAiCharacter> AiActorClassCPlayer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemies", Meta = (AllowPrivateAccess = true)) TSubclassOf<ABaseAiCharacter> AiActorClassCObjective;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemies", Meta = (AllowPrivateAccess = true)) TSubclassOf<class AWeaponSpawnerChest> ChestClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemies", Meta = (AllowPrivateAccess = true)) UAiCharacterData* RegularZombieClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemies", Meta = (AllowPrivateAccess = true)) UAiCharacterData* SpitterZombieClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemies", Meta = (AllowPrivateAccess = true)) UAiCharacterData* BossZombieClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemies", Meta = (AllowPrivateAccess = true)) uint8 MaxEnemies = 24;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemies", Meta = (AllowPrivateAccess = true)) uint8 MaxEnemiesPerSpawnWave = 4;

	FTimerHandle RoundCooldownHandler;
	FTimerHandle RoundDelayHandler;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DEBUG_VISIBLE", Meta = (AllowPrivateAccess = true)) uint8 EnemiesAlive = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DEBUG_VISIBLE", Meta = (AllowPrivateAccess = true)) int EnemiesToSpawn = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DEBUG_VISIBLE", Meta = (AllowPrivateAccess = true)) int CurrentRound = 1;

	bool bPendingRoundRestart = true;
	float SpawnerTimer = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Regions", Meta = (AllowPrivateAccess = true)) TArray<ARegion*> regions;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Regions", Meta = (AllowPrivateAccess = true)) ARegion* currentRegion;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", Meta = (AllowPrivateAccess = true)) FPlayerStats PlayerStats;

	void BeginRound();
	void BeginRoundDelay();
	void EndRound();
	
	UFUNCTION(CallInEditor) void SpawnAirDrop();

	bool TrySpawnEnemy();

	int FindAvailableEnemy();
	void SpawnEnemy(uint8 EnemyIndex, const FVector& Position);

	bool CheckLineOfSight(const FVector& PawnLoc, const FVector& SpawnPoint) const;
	bool IsLookingAtDir(const FVector& PawnDir, const FVector& DirToPoint) const;

	void SetRegions();
	void GetMainController();
	UFUNCTION(CallInEditor) void DEBUG_KILL_ENEMY();

	UFUNCTION(BlueprintCallable) FPlayerStats GetStats() const { return PlayerStats; }
	
protected:
	virtual void BeginPlay() override;
	
public:
	ATGPGameModeBase();

	AActor* GetCurrentRegionObjective() const {return currentRegion->GetObjective();}
	FString GetCurrentRegionName() const {return currentRegion->GetName();}

	virtual void Tick(float DeltaSeconds) override;

	void OnEnemyKilled(ABaseAiCharacter* Enemy); // TODO: REPLACE WITH AI CLASS
	
	template<class T>
	T* CreateItemByShortName(const FString& ItemShortName, const int Amount = 1, AActor* Owner = nullptr);

	template<class T>
	T* CreateItemByUniqueId(int UniqueId, const int Amount = 1, AActor* Owner = nullptr);

	UPROPERTY()
	class AMainPlayerController* MainPlayerController;
};

template <class T>
T* ATGPGameModeBase::CreateItemByShortName(const FString& ItemShortName, const int Amount, AActor* Owner)
{
	if (TIsDerivedFrom<T, UBaseItem>::IsDerived)
	{
		UItemInfo* Info = Cast<UBaseGameInstance>(GetGameInstance())->FindInfoShortName(ItemShortName);

		if (Info != nullptr)
		{
			T* NewItem = NewObject<T>(Owner == nullptr ? this : Owner);
			Cast<UBaseItem>(NewItem)->Init(Info, Amount);

			return NewItem;
		}
	}
	
	return nullptr;
}

template <class T>
T* ATGPGameModeBase::CreateItemByUniqueId(int UniqueId, const int Amount, AActor* Owner)
{
	if (TIsDerivedFrom<T, UBaseItem>::IsDerived)
	{
		UItemInfo* Info = Cast<UBaseGameInstance>(GetGameInstance())->FindInfoUniqueId(UniqueId);

		if (Info != nullptr)
		{
			T* NewItem = NewObject<T>(Owner == nullptr ? this : Owner);
			Cast<UBaseItem>(NewItem)->Init(Info, Amount);

			return NewItem;
		}
	}
	
	return nullptr;
}