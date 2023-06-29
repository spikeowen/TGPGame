#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Item/ItemInfo.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "BaseGameInstance.generated.h"

class UItemInfo;

UCLASS()
class TGP_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UPROPERTY() TArray<UItemInfo*> ItemInfos;

	TSharedPtr<FOnlineSessionSettings> SessionSettings;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;

	FOnSessionInviteReceivedDelegate SessionInviteReceivedDelegate;
	FDelegateHandle SessionInviteReceivedDelegateHandle;

	FOnSessionUserInviteAcceptedDelegate SessionInviteAcceptedDelegate;
	FDelegateHandle SessionInviteAcceptedDelegateHandle;
	
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	void OnSessionInviteAccepted(const bool bSuccessful, int32 LocalPlayer, TSharedPtr<const FUniqueNetId> PersonInviting, const FOnlineSessionSearchResult& SessionToJoin);
	void OnSessionInviteReceivedFromFriend(const FUniqueNetId& UserId, const FUniqueNetId& FromId, const FString& AppId, const FOnlineSessionSearchResult& Result);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnCreateSessionCompleted(FName SessionName, bool bSuccessful);
	
	bool JoinFriendSession(TSharedPtr<const FUniqueNetId> LocalUserId, FName SessionName, const FOnlineSessionSearchResult& Result);

	void OnLoadedInfos();
	void LoadInfos();
	
public:
	UBaseGameInstance();
	
	virtual void Init() override;
	virtual void Shutdown() override;
	
	UFUNCTION(BlueprintCallable) void CreateSession(int32 NumPublicConnections, bool bIsLanMatch);
	bool SendSessionInvite(APlayerController *PlayerController, const FUniqueNetId& FriendId);
	
	FOnCreateSessionComplete OnCreateSessionCompleteEvent;
	UTexture2D* GetFriendAvatar(TSharedRef<const FUniqueNetId> FriendId);

	UItemInfo* FindInfoShortName(const FString& ItemShortName) const;
	UItemInfo* FindInfoUniqueId(int UniqueId) const;

	int32 GetRandomItemIdOfCategory(enum EItemCategory ItemCategory);
	int32 GetRandomItemIdOfCategory(enum EItemCategory ItemCategory, ERarity ItemRarity);
};
