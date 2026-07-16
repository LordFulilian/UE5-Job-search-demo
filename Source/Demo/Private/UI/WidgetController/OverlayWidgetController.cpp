#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/PlayerAbilitySystemComponent.h"
#include "AbilitySystem/PlayerAttributeSet.h"
#include "Chaos/Deformable/MuscleActivationConstraints.h" 
#include "Player/OPlayerState.h" 
#include "Components/ExpComponent.h" 

void UOverlayWidgetController::BroadcastInitialValues()
{
    // Broadcast values owned by the base controller first.
    Super::BroadcastInitialValues();

    // Broadcast initial health values.
    const UPlayerAttributeSet* PlayerAttributeSet = CastChecked<UPlayerAttributeSet>(AttributeSet);
    OnHealthChanged.Broadcast(PlayerAttributeSet->GetHealth());
    OnMaxHealthChanged.Broadcast(PlayerAttributeSet->GetMaxHealth());

    // The experience component can also provide initial XP values.
    if (AOPlayerState* OPlayerState = Cast<AOPlayerState>(PlayerState))
    {
        if (OPlayerState->ExpComponent)
        {
            // Enable this once the component exposes the required getters.
            // OnXPChangedDelegate.Broadcast(OPlayerState->ExpComponent->GetCurrentXP(), OPlayerState->ExpComponent->GetXPToNext());
        }
    }
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
    // Bind dependencies owned by the base controller first.
    Super::BindCallbacksToDependencies();

    const UPlayerAttributeSet* PlayerAttributeSet = CastChecked<UPlayerAttributeSet>(AttributeSet);
    
    // Forward health changes to the overlay.
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PlayerAttributeSet->GetHealthAttribute()).AddLambda(
    [this](const FOnAttributeChangeData& Data)
    {
       OnHealthChanged.Broadcast(Data.NewValue);  
    });
    
    // Forward maximum-health changes to the overlay.
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PlayerAttributeSet->GetMaxHealthAttribute()).AddLambda(
    [this](const FOnAttributeChangeData& Data)
    {
        OnMaxHealthChanged.Broadcast(Data.NewValue);  
    });
    
    // Convert gameplay-effect asset tags into UI messages.
    Cast<UPlayerAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
    [this](const FGameplayTagContainer& AssetTag)
    {
       for (const FGameplayTag& Tag : AssetTag)
       {     
          FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
          if (Tag.MatchesTag(MessageTag))
          {
             const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
             MessageWidgetRowDelegate.Broadcast(*Row);
          }
       }
    });

    // Forward experience-component events.
    if (AOPlayerState* OPlayerState = Cast<AOPlayerState>(PlayerState))
    {
        if (OPlayerState->ExpComponent)
        {
            // Adapt component delegates to widget-controller delegates.
            OPlayerState->ExpComponent->OnExperienceChanged.AddDynamic(this, &UOverlayWidgetController::XPChangedCallback);
            OPlayerState->ExpComponent->OnLeveledUp.AddDynamic(this, &UOverlayWidgetController::LevelUpCallback);
        }
    }
}


void UOverlayWidgetController::XPChangedCallback(int32 CurrentXP, int32 XPToNext)
{
    OnXPChangedDelegate.Broadcast(CurrentXP, XPToNext);
}

void UOverlayWidgetController::LevelUpCallback(int32 NewLevel)
{
    OnLevelUpDelegate.Broadcast(NewLevel);
}
