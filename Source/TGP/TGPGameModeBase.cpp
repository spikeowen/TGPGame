#include "TGPGameModeBase.h"

#include "DrawDebugHelpers.h"
#include "MainPlayerController.h"
#include "FP_FirstPerson/FP_FirstPersonCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Ai/BaseAiCharacter.h"
#include "Ai/AiCharacterData.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapons/WeaponGenerators/WeaponSpawnerChest.h"

ATGPGameModeBase::ATGPGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATGPGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	EnemyPool.Reserve(MaxEnemies);

	FActorSpawnParameters SpawnParams;
	SpawnParams.bNoFail = true;

	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, "Amogus");
	
	for (uint8 i = 0; i < MaxEnemies; i++) // Init Pool
	{
		if(i%2==0)
		{
			EnemyPool.Add(GetWorld()->SpawnActor<ABaseAiCharacter>(AiActorClassCPlayer ? AiActorClassCPlayer : ABaseAiCharacter::StaticClass(), FVector(0.f, 0.f, 10000.f) + (FVector(200.f, 0.f, 0.f) * i), FRotator(), SpawnParams));
		}
		else
		{
			EnemyPool.Add(GetWorld()->SpawnActor<ABaseAiCharacter>(AiActorClassCObjective ? AiActorClassCObjective :  ABaseAiCharacter::StaticClass(), FVector(0.f, 0.f, 10000.f) + (FVector(200.f, 0.f, 0.f) * i), FRotator(), SpawnParams));
		}
		
		EnemyPool[i]->SetHidden(true);
	}

	GetWorld()->GetTimerManager().SetTimer(RoundCooldownHandler, this, &ATGPGameModeBase::BeginRound, CooldownBetweenRounds, false);

	if(UGameplayStatics::GetPlayerController(GetWorld(), 0)->IsA(AMainPlayerController::StaticClass()))
	{
		MainPlayerController = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}
}

void ATGPGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (EnemiesToSpawn > 0)
	{
		SpawnerTimer += DeltaSeconds;

		if (SpawnerTimer >= SpawnTimer)
		{
			const int WaveCount = EnemiesToSpawn < MaxEnemiesPerSpawnWave ? EnemiesToSpawn : MaxEnemiesPerSpawnWave;
			
			for (uint8 i = 0; i < WaveCount; i++)
				if(!TrySpawnEnemy())
					break;

			SpawnerTimer = 0.f;
		}	
	}
	else if (EnemiesAlive <= 0 && !bPendingRoundRestart)
	{
		bPendingRoundRestart = true;
		SpawnerTimer = 0.f;
		EndRound();
	}
}

void ATGPGameModeBase::BeginRound()
{
	if(regions.Num()>0)
	{
		currentRegion = regions[UKismetMathLibrary::RandomInteger(regions.Num())];
		currentRegion->BeginRound();
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, currentRegion->GetName());
	}
	GetWorld()->GetTimerManager().SetTimer(RoundDelayHandler, this, &ATGPGameModeBase::BeginRoundDelay, CooldownBetweenRounds, false);

	if(MainPlayerController)
	{
		MainPlayerController->SpawnRegion = currentRegion->GetName();
		MainPlayerController->UpdateEnemyRegion();
	}	
}

void ATGPGameModeBase::BeginRoundDelay()
{
	
	EnemiesToSpawn = (int)((SpawnIncreaseExponential * CurrentRound) * 24);
	SpawnerTimer = 0.f;
	bPendingRoundRestart = false;
}

void ATGPGameModeBase::EndRound()
{
	SpawnAirDrop();
	
	currentRegion->EndOfRound();
	CurrentRound++;

	PlayerStats.RoundAchieved = CurrentRound;
	
	GetWorld()->GetTimerManager().SetTimer(RoundCooldownHandler, this, &ATGPGameModeBase::BeginRound, CooldownBetweenRounds, false); // Begin Cooldown between rounds
}

void ATGPGameModeBase::SpawnAirDrop()
{
	if (const APlayerController* Player = GetWorld()->GetFirstPlayerController())
	{
		if (const APawn* Pawn = Player->GetPawn())
		{
			const FVector SpawnPoint = Pawn->GetActorLocation() + (FVector::UpVector * 5000.f) + FVector(FMath::RandRange(-200.f, 200.f), FMath::RandRange(-200.f, 200.f), FMath::RandRange(-200.f, 200.f));
			GetWorld()->SpawnActor<AWeaponSpawnerChest>(ChestClass, SpawnPoint, FRotator());
		}
	}
	
}

bool ATGPGameModeBase::TrySpawnEnemy()
{
	const int AvailableIndex = FindAvailableEnemy();

	if (AvailableIndex == -1)
		return false;
	
	for (int i = 0; i < 100; i++)
	{
		FVector AttemptedSpawnPoint;
		if(currentRegion!=nullptr)
		{
			AttemptedSpawnPoint = currentRegion->GetRandomPointInRegion();
		}
		else
		{
			AttemptedSpawnPoint = FVector(FMath::RandRange(SpawnMinRange.X, SpawnMaxRange.X), FMath::RandRange(SpawnMinRange.Y, SpawnMaxRange.Y), 10000.f);
		}
		 

		DrawDebugLine(GetWorld(),AttemptedSpawnPoint, AttemptedSpawnPoint + (FVector::DownVector * 15000.f), FColor::Red, false, 1.f);
		
		FHitResult Result;
		if (GetWorld()->LineTraceSingleByChannel(Result, AttemptedSpawnPoint, AttemptedSpawnPoint + (FVector::DownVector * 15000.f), ECollisionChannel::ECC_Visibility))
		{
			if (Result.Actor->Tags.Contains("Ground"))
			{
				FHitResult Result2;
				if (GetWorld()->LineTraceSingleByChannel(Result2, Result.Location, Result.Location + FVector(0.f, 0.f, 5000.f), ECollisionChannel::ECC_WorldStatic))
				{
					DrawDebugSphere(GetWorld(), Result.Location + FVector(0.f, 0.f, 75.f), 50.f, 8, FColor::Red, true); // Remove this <- Used for debugging zombs spawning in walls
					continue;
				}
				
				//FHitResult Res;
				//if (GetWorld()->SweepSingleByChannel(Res, SpherePos, SpherePos, FQuat::Identity, ECC_WorldStatic, MyColSphere))
				//{
				//	DrawDebugSphere(GetWorld(), SpherePos, MyColSphere.GetSphereRadius(), 8, FColor::Red, true);
				//	continue;
				//}
				
				bool bCanSpawn = false;

				for (int j = 0; j < GetNumPlayers(); j++)
				{
					if (const APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), j))
					{
						if (const AFP_FirstPersonCharacter* Pawn = Cast<AFP_FirstPersonCharacter>(Controller->GetPawn()))
						{
							const FVector PawnLoc = Pawn->GetActorLocation(), SpawnLoc = Result.Location + FVector(0.f, 0.f, 44.f);
							
							if (FVector::Dist(SpawnLoc, PawnLoc) >= MinSpawnDistFromPlayer)
							{
								FVector SpawnDir = PawnLoc - SpawnLoc;
								SpawnDir.Normalize();

								DrawDebugLine(GetWorld(),PawnLoc, SpawnLoc, FColor::Blue, false, 1.f);
								
								if (CheckLineOfSight(PawnLoc, SpawnLoc) || IsLookingAtDir(Pawn->GetActorForwardVector(), SpawnDir))
									bCanSpawn = true;
							}
							else
							{
								bCanSpawn = false;
								break;
							}
						}
					}
				}

				if (bCanSpawn)
				{
					SpawnEnemy(AvailableIndex, Result.Location);
					return true;
				}
			}
		}
	}

	return false;
}

int ATGPGameModeBase::FindAvailableEnemy()
{
	for	(uint8 i = 0; i < EnemyPool.Num(); i++)
		if (EnemyPool[i]->bIsDead)
			return i;

	return -1;
}

void ATGPGameModeBase::OnEnemyKilled(ABaseAiCharacter* Enemy)
{
	EnemiesAlive--;
	PlayerStats.EnemiesKilled++;
}

void ATGPGameModeBase::SpawnEnemy(uint8 EnemyIndex, const FVector& Position) // TODO: RESET OTHER PROPERTIES LIKE HEALTH
{
	if (EnemyIndex < EnemyPool.Num())
	{
		EnemyPool[EnemyIndex]->SpawnEnemy(Position);
	
		EnemiesAlive++;
		EnemiesToSpawn--;
	}
}

bool ATGPGameModeBase::IsLookingAtDir(const FVector& PawnDir, const FVector& DirToPoint) const
{
	return FVector::DotProduct(PawnDir, DirToPoint) > 0.f;
}

void ATGPGameModeBase::SetRegions()
{
	
}

void ATGPGameModeBase::GetMainController()
{
}

void ATGPGameModeBase::DEBUG_KILL_ENEMY()
{
	for (uint8 i = 0; i < EnemyPool.Num(); i++)
	{
		if (EnemyPool[i]->IsAlive())
		{
			EnemyPool[i]->OnEnemyDied(nullptr);
			OnEnemyKilled(EnemyPool[i]);
			break;
		}
	}
}

bool ATGPGameModeBase::CheckLineOfSight(const FVector& PawnLoc, const FVector& SpawnPoint) const
{
	FHitResult Result;
	return GetWorld()->LineTraceSingleByChannel(Result, PawnLoc, SpawnPoint, ECollisionChannel::ECC_Visibility);
}
