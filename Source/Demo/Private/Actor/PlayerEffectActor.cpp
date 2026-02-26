

#include "Actor/PlayerEffectActor.h"
#include  "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/PlayerAttributeSet.h"



APlayerEffectActor::APlayerEffectActor()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootSceneComponent"));
}


void APlayerEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr)return;
	
	check(GameplayEffectClass)
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass,1.f,EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle =  TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnOverlap )
	{
		ActiveEffectHandles.Add(ActiveEffectHandle,TargetASC);
	}	
	
}

void APlayerEffectActor::OnOverlap(AActor* TargetActor)
{
    // 1. 瞬间效果 (Instant)
    if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
    {
       ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
    }
    
    // 2. 持续效果 (Duration)
    if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
    {
       ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
    }
    
    // 3. 【你漏掉的】无限效果 (Infinite)
    if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
    {
       ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
    }
}

void APlayerEffectActor::OnEndOverlap(AActor* TargetActor)
{
    // 1. 【修复】离开时触发瞬间效果 
    if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
       ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
    }
    
    // 2. 【修复】离开时触发持续效果
    if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
       ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
    }
    
    // 3. 【你漏掉的】离开时触发无限效果
    if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
       ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
    }

    // 4. 离开时移除无限效果的逻辑
    if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnOverlap) // (注：这里你的枚举名虽然叫Overlap，但写在EndOverlap里逻辑是合理的)
    {
       UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
       if (!IsValid(TargetASC)) return;
       
       // 在这里声明这个空数组，当做“垃圾桶”
       TArray<FActiveGameplayEffectHandle> HandlesToRemove; 
       
       for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
       {
          if (TargetASC == HandlePair.Value)
          {
             // 告诉 GAS 系统：把玩家身上的这个效果拿掉！
             TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
             
             // 把要删的 Handle 记录到垃圾桶数组里
             HandlesToRemove.Add(HandlePair.Key); 
          }
       }
       
       // 遍历垃圾桶，把我们自己的 Map 里的记录也清理干净
       for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
       {
          ActiveEffectHandles.Remove(Handle);
       }
    }
}

