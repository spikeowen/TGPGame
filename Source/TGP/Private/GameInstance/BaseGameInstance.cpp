// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstance/BaseGameInstance.h"
#include "Engine/AssetManager.h"
#include "Item/ItemInfo.h"

#include "steam/steam_api.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UBaseGameInstance::UBaseGameInstance() : CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UBaseGameInstance::OnCreateSessionCompleted)),
	SessionInviteReceivedDelegate(FOnSessionInviteReceivedDelegate::CreateUObject(this, &UBaseGameInstance::OnSessionInviteReceivedFromFriend)),
	SessionInviteAcceptedDelegate(FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UBaseGameInstance::OnSessionInviteAccepted)),
	OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UBaseGameInstance::OnJoinSessionComplete))
{
	
}

void UBaseGameInstance::OnLoadedInfos()
{
	const UAssetManager& AssetManager = UAssetManager::Get();

	TArray<UObject*> LoadedInfos;
	AssetManager.GetPrimaryAssetObjectList(FPrimaryAssetType("ItemInfo"), LoadedInfos);

	ItemInfos.Reserve(LoadedInfos.Num());

	for (UObject* Obj : LoadedInfos)
		ItemInfos.Add(Cast<UItemInfo>(Obj));
}

void UBaseGameInstance::LoadInfos()
{
	UAssetManager& AssetManager = UAssetManager::Get();

	if (AssetManager.IsValid())
	{
		TArray<FPrimaryAssetId> PrimaryIds;
		
		AssetManager.GetPrimaryAssetIdList(FPrimaryAssetType("ItemInfo"), PrimaryIds);
		AssetManager.LoadPrimaryAssets(PrimaryIds, TArray<FName>(), FStreamableDelegate::CreateUObject(this, &UBaseGameInstance::OnLoadedInfos));
		
		ItemInfos.Reserve(PrimaryIds.Num());

		UE_LOG(LogTemp, Warning, TEXT("LOADED %d"), PrimaryIds.Num());
		
		for (FPrimaryAssetId Id : PrimaryIds)
			ItemInfos.Add(Cast<UItemInfo>(AssetManager.GetPrimaryAssetPath(Id).TryLoad()));
	}
}

void UBaseGameInstance::Init()
{
	Super::Init();
	
	if (SteamAPI_Init())
	{
		IOnlineSessionPtr Session = Online::GetSessionInterface();//OnlineSub->GetSessionInterface();

		if (Session.IsValid())
		{
			SessionInviteAcceptedDelegateHandle = Session->AddOnSessionUserInviteAcceptedDelegate_Handle(SessionInviteAcceptedDelegate);
			SessionInviteReceivedDelegateHandle = Session->AddOnSessionInviteReceivedDelegate_Handle(SessionInviteReceivedDelegate);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("FAILED TO INIT STEAM"));
	}

	LoadInfos();
}

void UBaseGameInstance::Shutdown()
{
	Super::Shutdown();

	UAssetManager& AssetManager = UAssetManager::Get();

	if (AssetManager.IsValid())
	{
		TArray<FPrimaryAssetId> PrimaryIds;
		
		AssetManager.GetPrimaryAssetIdList(FPrimaryAssetType("ItemInfo"), PrimaryIds);
		AssetManager.UnloadPrimaryAssets(PrimaryIds);
	}
}

UItemInfo* UBaseGameInstance::FindInfoShortName(const FString& ItemShortName) const
{
	for (UItemInfo* Info : ItemInfos)
		if (Info->ItemShortName == ItemShortName)
			return Info;

	return nullptr;
}

UItemInfo* UBaseGameInstance::FindInfoUniqueId(int UniqueId) const
{
	for (int i = 0; i < ItemInfos.Num(); i++)
	{
		if (ItemInfos[i]->UniqueId == UniqueId)
		{
			return ItemInfos[i];
		}
	}

	return nullptr;
}

int32 UBaseGameInstance::GetRandomItemIdOfCategory(EItemCategory ItemCategory)
{
	TArray<int> FoundIndexes;

	for (int i = 0; i < ItemInfos.Num(); i++)
		if (ItemInfos[i]->ItemCategory == ItemCategory)
			FoundIndexes.Add(i);

	if (FoundIndexes.Num() > 0)
		return ItemInfos[FoundIndexes[FMath::RandRange(0, FoundIndexes.Num() - 1)]]->UniqueId;
	
	return -1;
}

int32 UBaseGameInstance::GetRandomItemIdOfCategory(EItemCategory ItemCategory, ERarity ItemRarity)
{
	TArray<int> FoundIndexes;
    
        for (int i = 0; i < ItemInfos.Num(); i++)
            if (ItemInfos[i]->ItemCategory == ItemCategory && ItemInfos[i]->ItemRarity == ItemRarity)
                FoundIndexes.Add(i);
    
        if (FoundIndexes.Num() > 0)
            return ItemInfos[FoundIndexes[FMath::RandRange(0, FoundIndexes.Num() - 1)]]->UniqueId;
        
        return -1;
}

void UBaseGameInstance::OnSessionInviteAccepted(const bool bSuccessful, int32 LocalPlayer, TSharedPtr<const FUniqueNetId> PersonInviting, const FOnlineSessionSearchResult& SessionToJoin)
{
	if (bSuccessful)
	{
		if (SessionToJoin.IsValid())
		{
			const TSharedPtr<const FUniqueNetId> Id = Online::GetIdentityInterface()->GetUniquePlayerId(0);

			if (Id.IsValid())
				JoinFriendSession(Id, NAME_GameSession, SessionToJoin);
		}
	}
}

void UBaseGameInstance::OnSessionInviteReceivedFromFriend(const FUniqueNetId& UserId, const FUniqueNetId& FromId, const FString& AppId, const FOnlineSessionSearchResult& Result)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Invite Received");
}

void UBaseGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();

		if (Session.IsValid())
		{
			Session->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
			APlayerController* const PlayerController = GetFirstLocalPlayerController();

			FString Ip;
			if (PlayerController && Session->GetResolvedConnectString(SessionName, Ip))
				PlayerController->ClientTravel(Ip, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UBaseGameInstance::OnCreateSessionCompleted(FName SessionName, bool bSuccessful)
{
	const IOnlineSessionPtr Session = Online::GetSessionInterface();

	OnCreateSessionCompleteEvent.Broadcast(SessionName, bSuccessful);

	if (Session)
		Session->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
}

bool UBaseGameInstance::JoinFriendSession(TSharedPtr<const FUniqueNetId> LocalUserId, FName SessionName, const FOnlineSessionSearchResult& Result)
{
	bool bSuccessful = false;

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		
		if(Session.IsValid() && LocalUserId.IsValid())
		{
			OnJoinSessionCompleteDelegateHandle = Session->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
			bSuccessful = Session->JoinSession(*LocalUserId, SessionName, Result);
		}
	}

	return  bSuccessful;
}

void UBaseGameInstance::CreateSession(int32 NumPublicConnections, bool bIsLanMatch)
{
	const IOnlineSessionPtr Session = Online::GetSessionInterface();

	if (!Session.IsValid())
	{
		OnCreateSessionCompleteEvent.Broadcast(NAME_GameSession, false);
		return;
	}

	SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->NumPrivateConnections = 0;
	SessionSettings->NumPublicConnections = NumPublicConnections;
	SessionSettings->bAllowInvites = true;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bAllowJoinViaPresenceFriendsOnly = true;
	SessionSettings->bIsDedicated = false;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bIsLANMatch = bIsLanMatch;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->Set(SETTING_MAPNAME, FString("Lobby"), EOnlineDataAdvertisementType::ViaOnlineService);

	CreateSessionCompleteDelegateHandle = Session->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	if (!Session->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings))
	{
		Session->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		OnCreateSessionCompleteEvent.Broadcast(NAME_GameSession, false);
	}
}

bool UBaseGameInstance::SendSessionInvite(APlayerController* PlayerController, const FUniqueNetId& FriendId)
{
	if (PlayerController &&  FriendId.IsValid())
	{
		IOnlineSessionPtr Session = Online::GetSessionInterface();
		ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerController->Player);

		TSharedPtr<const FUniqueNetId> Id = Online::GetIdentityInterface()->GetUniquePlayerId(0);

		if (Session && Player)
			if(Session->SendSessionInviteToFriend(Player->GetControllerId(), NAME_GameSession, FriendId))
				return true;
	}
	
	return false;
}

UTexture2D* UBaseGameInstance::GetFriendAvatar(TSharedRef<const FUniqueNetId> FriendId)
{
	if (FriendId->IsValid() && FriendId->GetType() == STEAM_SUBSYSTEM)
	{
		if (SteamAPI_Init())
		{
			uint64 SteamID = *((uint64*)FriendId->GetBytes());

			int Picture = SteamFriends()->GetMediumFriendAvatar(SteamID);

			if (Picture == -1)
				return nullptr;

			uint32 Width = 0;
			uint32 Height = 0;

			SteamUtils()->GetImageSize(Picture, &Width, &Height);

			if (Width > 0 && Height > 0)
			{
				uint8 *AvatarRGBA = new uint8[Width * Height * 4];
				
				SteamUtils()->GetImageRGBA(Picture, AvatarRGBA, 4 * Height * Width * sizeof(char));

				UTexture2D* Avatar = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);
				uint8* MipData = (uint8*)Avatar->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(MipData, AvatarRGBA, Height * Width * 4);
				Avatar->PlatformData->Mips[0].BulkData.Unlock();
				
				delete[] AvatarRGBA;
				
				Avatar->PlatformData->SetNumSlices(1);
				Avatar->NeverStream = true;

				Avatar->UpdateResource();
				
				return Avatar;
			}
		}
	}
	
	return nullptr;
}