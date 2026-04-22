// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/EnemyCharacter.h"
#include "AbilitySystem/PlayerAttributeSet.h"
#include "AbilitySystem/PlayerAbilitySystemComponent.h"
#include "AbilitySystem/PlayerAbilitySystemLibrary.h"
#include "Chaos/Deformable/MuscleActivationConstraints.h"
#include "Components/MeshComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
	// 创建能力系统组件
	AbilitySystemComponent = CreateDefaultSubobject<UPlayerAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// 构建属性集
	AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("AttributeSet"));
	
	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(GetRootComponent());
}

// 
void AEnemyCharacter::ToggleHighlight_Implementation(bool bActive)
{
	// 🔴 1. 改为获取所有的 网格体组件 (MeshComponent)
	TArray<UMeshComponent*> MeshComponents;
	GetComponents<UMeshComponent>(MeshComponents);
    
	// 🔴 2. 遍历所有的网格体组件
	for (UMeshComponent* Component : MeshComponents)
	{
		if (Component)
		{
			// 开启/关闭自定义深度
			Component->SetRenderCustomDepth(bActive);

			// 设置模具值 (Stencil Value) 为 250 (红色)
			if (bActive)
			{
				Component->SetCustomDepthStencilValue(250);
			}
		}
	}
}
void AEnemyCharacter::BeginPlay()

{

	Super::BeginPlay();

	InitAbilityActorInfo();


	HealthBar->InitWidget();



	if (UPlayerUserWidget * PlayerUserWidget= Cast<UPlayerUserWidget>(HealthBar->GetUserWidgetObject()))

	{

		PlayerUserWidget->SetWidgetController(this);

	}

	if (const UPlayerAttributeSet *PlayerAS= Cast<UPlayerAttributeSet>(AttributeSet))

	{

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PlayerAS->GetHealthAttribute()).AddLambda(

		[this](const FOnAttributeChangeData& Data)

		{

		OnHealthChanged.Broadcast(Data.NewValue);

		}

		);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PlayerAS->GetMaxHealthAttribute()).AddLambda(

		[this](const FOnAttributeChangeData& Data)

		{

		OnMaxHealthChanged.Broadcast(Data.NewValue);

		}

		);


		OnHealthChanged.Broadcast(PlayerAS->GetHealth());

		OnMaxHealthChanged.Broadcast(PlayerAS->GetMaxHealth());

	}

}
void AEnemyCharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UPlayerAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	
	InitialzeDefaultAttributes();
}

int32 AEnemyCharacter::GetPlayerLevel()
{
	return Level;
}

void AEnemyCharacter::InitialzeDefaultAttributes() 
{
	UPlayerAbilitySystemLibrary::InitialzeDefaultAttributes(this,CharacterClass,Level,AbilitySystemComponent);
}


