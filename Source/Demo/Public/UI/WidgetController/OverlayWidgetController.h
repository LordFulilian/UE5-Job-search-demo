// Rylan

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/PlayerUserWidget.h"
#include "UI/WidgetController/PlayerWidgetController.h"
#include "OverlayWidgetController.generated.h"

USTRUCT()
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FText  MessageText = FText();
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<class UPlayerUserWidget> MessageWidget;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSinature,float,NewValue);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature,FUIWidgetRow,Row);



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
	FOnAttributeChangedSinature OnHealthChanged;	
	
	UPROPERTY(BlueprintAssignable,Category="GAS|Attempts")
	FOnAttributeChangedSinature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable,Category="GAS|Message")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;
	
	
	
protected:
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;
	

	
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable,const FGameplayTag& Tag);
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(),TEXT(""));
}
