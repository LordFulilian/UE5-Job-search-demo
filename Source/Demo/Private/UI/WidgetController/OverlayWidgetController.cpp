#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/PlayerAbilitySystemComponent.h"
#include "AbilitySystem/PlayerAttributeSet.h"
#include "Chaos/Deformable/MuscleActivationConstraints.h" 
#include "Player/OPlayerState.h" 
#include "Components/ExpComponent.h" 

void UOverlayWidgetController::BroadcastInitialValues()
{
    // 让父类把基础数据发出去
    Super::BroadcastInitialValues();

    // 广播初始血量数据
    const UPlayerAttributeSet* PlayerAttributeSet = CastChecked<UPlayerAttributeSet>(AttributeSet);
    OnHealthChanged.Broadcast(PlayerAttributeSet->GetHealth());
    OnMaxHealthChanged.Broadcast(PlayerAttributeSet->GetMaxHealth());

    // 🔴 广播初始经验值和等级 (可选：如果你希望 UI 刚加载就显示正确的经验条)
    if (AOPlayerState* OPlayerState = Cast<AOPlayerState>(PlayerState))
    {
        if (OPlayerState->ExpComponent)
        {
            // 提示：如果你 ExpComponent 里有对应的 Get 函数，可以解开这里的注释
            // OnXPChangedDelegate.Broadcast(OPlayerState->ExpComponent->GetCurrentXP(), OPlayerState->ExpComponent->GetXPToNext());
        }
    }
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
    // 养成好习惯：绑定回调时也先喊一声父类
    Super::BindCallbacksToDependencies();

    const UPlayerAttributeSet* PlayerAttributeSet = CastChecked<UPlayerAttributeSet>(AttributeSet);
    
    // 1. 绑定血量变化
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PlayerAttributeSet->GetHealthAttribute()).AddLambda(
    [this](const FOnAttributeChangeData& Data)
    {
       OnHealthChanged.Broadcast(Data.NewValue);  
    });
    
    // 2. 绑定最大血量变化
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PlayerAttributeSet->GetMaxHealthAttribute()).AddLambda(
    [this](const FOnAttributeChangeData& Data)
    {
        OnMaxHealthChanged.Broadcast(Data.NewValue);  
    });
    
    // 3. 绑定 Tag 消息弹窗
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

    // 🔴 4. 绑定经验组件的事件！
    if (AOPlayerState* OPlayerState = Cast<AOPlayerState>(PlayerState))
    {
        if (OPlayerState->ExpComponent)
        {
            // 将后端的 ExpComponent 信号，绑定到我们的 Callback 函数上
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
