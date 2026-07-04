#include "Components/InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

UInventoryComponent::UInventoryComponent()
{
    // 背包组件是纯数据容器，不需要每帧刷新，关掉 Tick 节省性能
    PrimaryComponentTick.bCanEverTick = false; 
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // 游戏开始时，按照容量初始化格子数组
    InventorySlots.SetNum(InventoryCapacity);
}

bool UInventoryComponent::GetItemStaticData(FName ItemID, FItemStaticData& OutItemData)
{
    if (ItemDataTable)
    {
        // 核心：基于 ID 去读取数据表对应行
        FItemStaticData* Data = ItemDataTable->FindRow<FItemStaticData>(ItemID, TEXT("Inventory Lookup"));
        if (Data)
        {
            OutItemData = *Data;
            return true;
        }
    }
    return false;
}

bool UInventoryComponent::AddItem(FName ItemID, int32 Amount)
{
    if (Amount <= 0 || ItemID == NAME_None) return false;

    FItemStaticData ItemData;
    if (!GetItemStaticData(ItemID, ItemData))
    {
        UE_LOG(LogTemp, Warning, TEXT("尝试添加不存在的物品ID: %s，请检查数据表！"), *ItemID.ToString());
        return false;
    }

    int32 FirstEmptyIndex = -1;

    // 遍历背包格子
    for (int32 i = 0; i < InventorySlots.Num(); ++i)
    {
        // 1. 尝试堆叠 (同一种物品且没达到最大堆叠数)
        if (InventorySlots[i].ItemID == ItemID && InventorySlots[i].Quantity < ItemData.MaxStackSize)
        {
            int32 SpaceLeft = ItemData.MaxStackSize - InventorySlots[i].Quantity;
            if (SpaceLeft >= Amount)
            {
                InventorySlots[i].Quantity += Amount;
                OnInventoryUpdated.Broadcast(); // 数据变了，大喇叭广播
                return true; 
            }
            else
            {
                // 如果这个格子装不完，把这个格子装满，剩下的数量继续往后找
                InventorySlots[i].Quantity += SpaceLeft;
                Amount -= SpaceLeft;
            }
        }
        // 2. 顺便记录一下遇到的第一个空格子，留作备用
        else if (InventorySlots[i].IsEmpty() && FirstEmptyIndex == -1)
        {
            FirstEmptyIndex = i;
        }
    }

    // 3. 如果之前的堆叠没装完（或者根本没法堆叠），且找到了空格子，放进新格子
    if (Amount > 0 && FirstEmptyIndex != -1)
    {
        InventorySlots[FirstEmptyIndex].ItemID = ItemID;
        InventorySlots[FirstEmptyIndex].Quantity = Amount;
        OnInventoryUpdated.Broadcast(); // 数据变了，大喇叭广播
        return true;
    }

    return false; // 背包满了，添加失败
}

bool UInventoryComponent::UseItemAtIndex(int32 SlotIndex)
{
    // 防御性编程：检查越界和空格子
    if (!InventorySlots.IsValidIndex(SlotIndex) || InventorySlots[SlotIndex].IsEmpty()) return false;

    FName ItemToUse = InventorySlots[SlotIndex].ItemID;
    FItemStaticData ItemData;
    
    if (GetItemStaticData(ItemToUse, ItemData))
    {
        // ==========================================
        // 🔴 GAS 终极联动：吃掉物品，赋予属性！
        // ==========================================
        if (ItemData.UsedGameplayEffect)
        {
            AActor* Owner = GetOwner();
            UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
            
            if (ASC)
            {
                // 像你写近战伤害那样，构造 Context 并应用 GE 给自己 (回血/加攻击等)
                FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
                ContextHandle.AddInstigator(Owner, Owner);
                
                FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ItemData.UsedGameplayEffect, 1.f, ContextHandle);
                if (SpecHandle.IsValid())
                {
                    ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                }
            }
        }

        // ==========================================
        // 扣除物品数量
        // ==========================================
        InventorySlots[SlotIndex].Quantity -= 1;
        
        // 如果用光了，把格子清空归位
        if (InventorySlots[SlotIndex].Quantity <= 0)
        {
            InventorySlots[SlotIndex].ItemID = NAME_None;
            InventorySlots[SlotIndex].Quantity = 0;
        }

        // 通知 UI 更新显示数量
        OnInventoryUpdated.Broadcast();
        return true;
    }

    return false;
}