// Rylan

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/PlayerUserWidget.h"
#include "UI/WidgetController/PlayerWidgetController.h"
#include "OverlayWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature,float,NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSignature,float,NewMaxHealth);


USTRUCT()
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FText  MessageText = FText();
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UPlayerUserWidget> MessageWidget;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};
/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class DEMO_API UOverlayWidgetController : public UPlayerWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
	
	UPROPERTY(BlueprintAssignable,Category="GAS|Attempts")
	FOnHealthChangedSignature OnHealthChanged;	
	
	UPROPERTY(BlueprintAssignable,Category="GAS|Attempts")
	FOnMaxHealthChangedSignature OnMaxHealthChanged;
	
protected:
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;
	
	void HealthChanged(const FOnAttributeChangeData& Data)const;
	void MaxHealthChanged(const FOnAttributeChangeData& Data)const;
};
