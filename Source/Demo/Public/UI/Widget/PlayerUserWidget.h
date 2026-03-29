// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUserWidget.generated.h"

// 前向声明，加快编译速度并防止头文件循环引用
class UButton;
class APlayerCharacter;

/**
 * 玩家 UI 基类
 * 负责与 WidgetController 交互，并处理基础的 UI 生命周期和关闭逻辑
 */
UCLASS()
class DEMO_API UPlayerUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 设置 UI 控制器
	UFUNCTION(BlueprintCallable, Category = "UI|Controller")
	void SetWidgetController(UObject* InWidgetController);

	// 持有 UI 控制器的引用
	UPROPERTY(BlueprintReadOnly, Category = "UI|Controller")
	TObjectPtr<UObject> WidgetController;

protected:
	// 供蓝图重写，当 Controller 设置完毕后触发
	UFUNCTION(BlueprintImplementableEvent, Category = "UI|Controller")
	void WidgetControllerSet();

	// --- UI 生命周期与关闭逻辑 ---

	// UI 构建时调用，用于绑定事件
	virtual void NativeOnInitialized() override;

	// UI 销毁时调用，用于清理引用的子页面
	virtual void NativeDestruct() override;

	// 关闭按钮。
	// 使用 BindWidgetOptional：如果子类蓝图中有叫 "close" 的按钮则绑定，没有也不会报错。
	// 注意：这里的变量名 "close" 必须与蓝图 Designer 面板里的控件名完全一致（区分大小写）！
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> close;

	// 关闭按钮的点击事件响应函数
	UFUNCTION()
	void OnCloseButtonClicked();

	// 用于保存可能打开的二级页面的引用
	// 如果你在主页面打开了二级页面，请把二级页面的实例赋值给这个变量
	UPROPERTY(BlueprintReadWrite, Category = "UI|SubPanel")
	TObjectPtr<UPlayerUserWidget> SubPanelInstance; 
};