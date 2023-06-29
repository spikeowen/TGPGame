#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactables/IInteractable.h"
#include "ItemActor.generated.h"

class UItemInfo;
class UBaseItem;
class UBoxComponent;
class UPointLightComponent;
class UWeaponInfo;
class UWeaponStatUIWidget;

UCLASS()
class TGP_API AItemActor : public AActor, public IIInteractable
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = true)) UStaticMeshComponent* ItemMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = true)) USkeletalMeshComponent* ItemSkeletalMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = true)) UPointLightComponent* ItemPointLight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = true)) class UWidgetComponent* StatWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DebugSpawn", meta=(AllowPrivateAccess = true)) UItemInfo* ItemInfoToSpawn;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DebugSpawn", meta=(AllowPrivateAccess = true)) TSubclassOf<UBaseItem> ItemClassToSpawn;
	
	UPROPERTY() APlayerController* _playerController;
	
	UPROPERTY(ReplicatedUsing=OnRep_UpdateItem, BlueprintReadOnly, Category = "Item", Meta = (AllowPrivateAccess = true)) UBaseItem* DefinedItem;

	UFUNCTION() void OnRep_UpdateItem();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
protected:
	virtual void BeginPlay() override;

	virtual void WidgetBillboard();

	void InitialiseWidgetText(const UWeaponInfo* info);
	
public:
	AItemActor();
	virtual void Tick(float DeltaTime) override;

	void LightColourSetup(const UWeaponInfo* info) const;

	USkeletalMeshComponent* GetSkeletalMesh() const { return ItemSkeletalMesh; }

	void AddInitialThrowForce(FVector dir, float force);

	virtual void StartHover() override;
	virtual void EndHover() override;
	
	UFUNCTION(BlueprintCallable) void Initialize(UBaseItem* Item);
	UFUNCTION(BlueprintCallable) UBaseItem* GetItem() const { return DefinedItem; }

	UFUNCTION(NetMulticast, Reliable) virtual void OnPickUp();
	void OnPickUp_Implementation();

	UFUNCTION(BlueprintImplementableEvent) void Test();
};
