#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" 
#include "ItemTypes.generated.h" 

// ==========================================
// 物品分类枚举
// ==========================================
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Material    UMETA(DisplayName = "材料"),
	Weapon      UMETA(DisplayName = "武器"),
	Quest       UMETA(DisplayName = "任务道具"),
	Apparatus   UMETA(DisplayName = "仪器")
};

// ==========================================
// 物品数据表结构体
// ==========================================
USTRUCT(BlueprintType)
struct FItemDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> ItemIcon;
};