#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "GameplayEffect.h" // GAS：为了使用 UGameplayEffect
#include "InventoryComponent.generated.h"

// =========================================================================
// 1. 静态数据表结构体 (定义物品DNA)
// =========================================================================
USTRUCT(BlueprintType)
struct FItemStaticData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Data")
    FName ItemID = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Data")
    FText ItemName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Data")
    UTexture2D* ItemIcon = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Data")
    UStaticMesh* ItemMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Data")
    int32 MaxStackSize = 99; // 最大堆叠数量

    // 🔴 GAS 联动核心：如果是药水，使用时给玩家赋予哪个 Gameplay Effect？
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|GAS")
    TSubclassOf<class UGameplayEffect> UsedGameplayEffect;
};

// =========================================================================
// 2. 动态格子结构体 (定义背包里的一个槽位)
// =========================================================================
USTRUCT(BlueprintType)
struct FInventorySlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Slot")
    FName ItemID = NAME_None; // 默认是没有物品

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Slot")
    int32 Quantity = 0;
    
    // 辅助函数：判断格子是否为空
    bool IsEmpty() const { return ItemID == NAME_None || Quantity <= 0; }
};

// UI 广播委托 (当背包变动时，立刻大喇叭通知 UMG 刷新界面)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);


// =========================================================================
// 3. 背包组件核心类
// =========================================================================
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEMO_API UInventoryComponent : public UActorComponent// ⚠️注意：把 YOURPROJECT_API 换成你实际的项目宏，比如 DEMO_API
{
    GENERATED_BODY()

public:	
    UInventoryComponent();

protected:
    virtual void BeginPlay() override;

public:
    // --- 核心配置 ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Config")
    int32 InventoryCapacity = 20; // 默认20个格子

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Config")
    UDataTable* ItemDataTable; // 将在蓝图里填入我们配好的数据表

    // --- 运行时数据 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|State")
    TArray<FInventorySlot> InventorySlots;

    // --- 广播事件 ---
    UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
    FOnInventoryUpdated OnInventoryUpdated;

    // --- 核心 API (暴露给蓝图调用) ---
    
    // 获取物品静态数据 (从数据表中查询)
    UFUNCTION(BlueprintCallable, Category = "Inventory|Logic")
    bool GetItemStaticData(FName ItemID, FItemStaticData& OutItemData);
    
    // 添加物品 (处理堆叠与寻找空位)
    UFUNCTION(BlueprintCallable, Category = "Inventory|Logic")
    bool AddItem(FName ItemID, int32 Amount);

    // 使用物品 (GAS 联动触发口)
    UFUNCTION(BlueprintCallable, Category = "Inventory|Logic")
    bool UseItemAtIndex(int32 SlotIndex);
};