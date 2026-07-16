#include "Components/InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

UInventoryComponent::UInventoryComponent()
{
    // Inventory is event-driven and does not require ticking.
    PrimaryComponentTick.bCanEverTick = false; 
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // Initialize the fixed-capacity slot array.
    InventorySlots.SetNum(InventoryCapacity);
}

bool UInventoryComponent::GetItemStaticData(FName ItemID, FItemStaticData& OutItemData)
{
    if (ItemDataTable)
    {
        // Resolve static item data by row identifier.
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

    // Search existing slots before allocating a new stack.
    for (int32 i = 0; i < InventorySlots.Num(); ++i)
    {
        // Fill compatible stacks up to their configured limit.
        if (InventorySlots[i].ItemID == ItemID && InventorySlots[i].Quantity < ItemData.MaxStackSize)
        {
            int32 SpaceLeft = ItemData.MaxStackSize - InventorySlots[i].Quantity;
            if (SpaceLeft >= Amount)
            {
                InventorySlots[i].Quantity += Amount;
                OnInventoryUpdated.Broadcast(); // Notify UI listeners.
                return true; 
            }
            else
            {
                // Continue searching when this stack cannot hold the remainder.
                InventorySlots[i].Quantity += SpaceLeft;
                Amount -= SpaceLeft;
            }
        }
        // Remember the first empty slot for any remaining items.
        else if (InventorySlots[i].IsEmpty() && FirstEmptyIndex == -1)
        {
            FirstEmptyIndex = i;
        }
    }

    // Place the remaining quantity in the first empty slot.
    if (Amount > 0 && FirstEmptyIndex != -1)
    {
        InventorySlots[FirstEmptyIndex].ItemID = ItemID;
        InventorySlots[FirstEmptyIndex].Quantity = Amount;
        OnInventoryUpdated.Broadcast(); // Notify UI listeners.
        return true;
    }

    return false; // Inventory is full.
}

bool UInventoryComponent::UseItemAtIndex(int32 SlotIndex)
{
    // Reject invalid or empty slots.
    if (!InventorySlots.IsValidIndex(SlotIndex) || InventorySlots[SlotIndex].IsEmpty()) return false;

    FName ItemToUse = InventorySlots[SlotIndex].ItemID;
    FItemStaticData ItemData;
    
    if (GetItemStaticData(ItemToUse, ItemData))
    {
        // ==========================================
        // Apply the consumable's configured gameplay effect.
        // ==========================================
        if (ItemData.UsedGameplayEffect)
        {
            AActor* Owner = GetOwner();
            UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
            
            if (ASC)
            {
                // Build an effect context and apply the effect to the owner.
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
        // Consume one item.
        // ==========================================
        InventorySlots[SlotIndex].Quantity -= 1;
        
        // Reset the slot after the stack is exhausted.
        if (InventorySlots[SlotIndex].Quantity <= 0)
        {
            InventorySlots[SlotIndex].ItemID = NAME_None;
            InventorySlots[SlotIndex].Quantity = 0;
        }

        // Refresh inventory UI.
        OnInventoryUpdated.Broadcast();
        return true;
    }

    return false;
}
