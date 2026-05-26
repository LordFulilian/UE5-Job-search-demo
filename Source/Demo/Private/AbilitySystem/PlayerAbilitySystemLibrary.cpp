// Rylan

#include "AbilitySystem/PlayerAbilitySystemLibrary.h"

#include "PlayerAbilityTypes.h"
#include "Game/PlayerGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/PlayerWidgetController.h"
#include "Player/OPlayerState.h"
#include "UI/HUD/PlayerHUD.h"

UOverlayWidgetController* UPlayerAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
    if (APlayerController *PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
       if (APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PC->GetHUD()))
       {
          AOPlayerState *PS = PC->GetPlayerState<AOPlayerState>();
          UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
          UAttributeSet* AS = PS->GetAttributeSet();
          const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
          return PlayerHUD->GetOverlayWidgetController(WidgetControllerParams);
       }
    }
    return nullptr;
}

UAttributeMenuWidgetController* UPlayerAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
    if (APlayerController *PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
       if (APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PC->GetHUD()))
       {
          AOPlayerState *PS = PC->GetPlayerState<AOPlayerState>();
          UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
          UAttributeSet* AS = PS->GetAttributeSet();
          const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
          return PlayerHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
       }
    }
    return nullptr;
}

void UPlayerAbilitySystemLibrary::InitialzeDefaultAttributes(const UObject* WorldContextObject,ECharacterClass CharacterClass, float Level,UAbilitySystemComponent*ASC)
{
    AActor* AvatarActor = ASC->GetAvatarActor();
	
    UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->CharacterClassDefaultInfo(CharacterClass);
    
    FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
    PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
    const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes,Level,PrimaryAttributesContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());
    
    FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
    SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
    const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes,Level,SecondaryAttributesContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());
    
    FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
    VitalAttributesContextHandle.AddSourceObject(AvatarActor);
    const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes,Level,VitalAttributesContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UPlayerAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
    UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
    if (CharacterClassInfo == nullptr) return;
    
    for (TSubclassOf<UGameplayAbility>AbilityClass : CharacterClassInfo->CommonAbilites)
    {
       FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);
       ASC->GiveAbility(AbilitySpec);
    }
}

UCharacterClassInfo* UPlayerAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
    APlayerGameModeBase *PlayerGameMode = Cast<APlayerGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
    if (PlayerGameMode == nullptr) return nullptr;
    
    return PlayerGameMode->CharacterClassInfo;
}

bool UPlayerAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle EffectContextHandle)
{
	if (const FPlayerGamePlayEffectContext* PlayEffectContext = static_cast<const FPlayerGamePlayEffectContext*>(EffectContextHandle.Get()))
	{
		return PlayEffectContext->IsCriticalHit();
	}
	return false;
}

void UPlayerAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle EffectContextHandle,
	bool bInIsCriticalHit)
{
	if (FPlayerGamePlayEffectContext* PlayEffectContext = static_cast<FPlayerGamePlayEffectContext*>(EffectContextHandle.Get()))
	{
		PlayEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}
