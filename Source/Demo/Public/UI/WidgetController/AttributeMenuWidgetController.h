// Rylan

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/PlayerWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"


struct FAttributeInfo; 

class UAttributeInfoAsset; 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAttributeInfo&, Info);

/**
 * */
UCLASS(BlueprintType, Blueprintable)
class DEMO_API UAttributeMenuWidgetController : public UPlayerWidgetController
{
	GENERATED_BODY()
    
public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;
    
	// UI 蓝图会监听这个委托来生成界面
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;
    
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAttributeInfoAsset> AttributeInfo;
};