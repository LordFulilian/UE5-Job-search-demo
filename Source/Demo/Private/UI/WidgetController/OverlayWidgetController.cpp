// Rylan

#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/PlayerAbilitySystemComponent.h"
#include "AbilitySystem/PlayerAttributeSet.h"
#include "Chaos/Deformable/MuscleActivationConstraints.h"


void UOverlayWidgetController::BroadcastInitialValues()
{
    // 🔴 关键修复：先让父类去把等级等通用数据发出去！
    Super::BroadcastInitialValues();

    // 然后再发 Overlay 专属的血量数据
    const UPlayerAttributeSet* PlayerAttributeSet = CastChecked<UPlayerAttributeSet>(AttributeSet);
    
    OnHealthChanged.Broadcast(PlayerAttributeSet->GetHealth());
    OnMaxHealthChanged.Broadcast(PlayerAttributeSet->GetMaxHealth());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
    // 🔴 养成好习惯：绑定回调时也先喊一声父类
    Super::BindCallbacksToDependencies();

    const UPlayerAttributeSet* PlayerAttributeSet = CastChecked<UPlayerAttributeSet>(AttributeSet);
    
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PlayerAttributeSet->GetHealthAttribute()).AddLambda(
    [this](const FOnAttributeChangeData& Data)
    {
       OnHealthChanged.Broadcast(Data.NewValue);  
    }  
    );
    
    // ⚠️ 注意：你这里的 MaxHealth 绑定的广播写错了，应该广播 OnMaxHealthChanged，而不是 OnHealthChanged！我顺手帮你修了：
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PlayerAttributeSet->GetMaxHealthAttribute()).AddLambda(
    [this](const FOnAttributeChangeData& Data)
    {
        OnMaxHealthChanged.Broadcast(Data.NewValue);  
    }   
    );
    
    Cast<UPlayerAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
    [this](const FGameplayTagContainer& AssetTag)
    {
       for (const FGameplayTag& Tag : AssetTag)
       {     
          FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
          if (Tag.MatchesTag(MessageTag))
          {
             const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable,Tag);
             MessageWidgetRowDelegate.Broadcast(*Row);
          }
       }
    }  
    );
}