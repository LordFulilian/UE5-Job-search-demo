// Rylan

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/PlayerUserWidget.h"
#include "UI/WidgetController/PlayerWidgetController.h"
#include "Components/ExpComponent.h" 
#include "Components/QuestComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/HeroUIInfo.h"
#include "OverlayWidgetController.generated.h"

// Forward declarations avoid circular includes for UI data assets.
class UAbilityInfo;
class UHeroUIInfo;
class UQuestDataAsset;

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag MessageTag = FGameplayTag();

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText MessageText = FText();
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<class UPlayerUserWidget> MessageWidget;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* Image = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FActivePartyChangedSignature,
    const TArray<FHeroSlotInfo>&, PartyMembers);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FQuestUIEventSignature, UQuestDataAsset*, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FTrackedQuestUISignature, bool, bHasTrackedQuest,
    FQuestRuntimeEntry, QuestEntry);

USTRUCT(BlueprintType)
struct FQuestNotificationData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FText Header;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FText QuestTitle;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FText RewardText;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FQuestNotificationSignature, FQuestNotificationData, Notification);

/**
 * */
UCLASS(BlueprintType, Blueprintable)
class DEMO_API UOverlayWidgetController : public UPlayerWidgetController
{
    GENERATED_BODY()

public:
    virtual void BroadcastInitialValues() override;
    virtual void BindCallbacksToDependencies() override;

    // Replays current quest state after Blueprint has bound its UI events.
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void RefreshQuestUI();

    UFUNCTION(BlueprintPure, Category = "Ability|Data")
    FPlayerAbilityInfo FindAbilityInfoForClass(
        TSubclassOf<UGameplayAbility> AbilityClass) const;
    
    UPROPERTY(BlueprintAssignable, Category = "GAS|XP")
    FOnExperienceChangedSignature OnXPChangedDelegate;

    UPROPERTY(BlueprintAssignable, Category = "GAS|Level")
    FOnLeveledUpSignature OnLevelUpDelegate;
    
    UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
    FOnAttributeChangedSignature OnHealthChanged;   
    
    UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
    FOnAttributeChangedSignature OnMaxHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "GAS|Message")
    FMessageWidgetRowSignature MessageWidgetRowDelegate;

    // Contains occupied slots only. Empty HUD portraits should not be created.
    UPROPERTY(BlueprintAssignable, Category = "Party")
    FActivePartyChangedSignature OnActivePartyChanged;

    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FQuestUIEventSignature OnQuestCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FQuestUIEventSignature OnQuestRewardClaimed;

    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FTrackedQuestUISignature OnTrackedQuestChanged;

    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FQuestNotificationSignature OnQuestNotification;
    
protected:
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
    TObjectPtr<UDataTable> MessageWidgetDataTable;

    // ========================================================
    // Static data exposed to the overlay Blueprint.
    // ========================================================
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data|Abilities")
    TObjectPtr<UAbilityInfo> AbilityInfoDataAsset;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data|Party")
    TObjectPtr<UHeroUIInfo> PartyInfoDataAsset;
    // ========================================================

    UFUNCTION()
    void XPChangedCallback(int32 CurrentXP, int32 XPToNext);

    UFUNCTION()
    void LevelUpCallback(int32 NewLevel);

    UFUNCTION()
    void PartyChangedCallback();

    UFUNCTION()
    void QuestCompletedCallback(UQuestDataAsset* Quest);

    UFUNCTION()
    void QuestRewardClaimedCallback(UQuestDataAsset* Quest);

    UFUNCTION()
    void QuestListChangedCallback();

    UFUNCTION()
    void TrackedQuestChangedCallback();

    void BroadcastTrackedQuest();
    
    template<typename T>
    T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
    return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}
