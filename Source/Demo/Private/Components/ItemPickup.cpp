
#include "Components/ItemPickup.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/PlayerCharacter.h" // 确保能认识玩家

AItemPickup::AItemPickup()
{
    PrimaryActorTick.bCanEverTick = false;

    // 1. 初始化根组件碰撞球，并强行设置足够大的半径！
    Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
    RootComponent = Sphere;
    Sphere->InitSphereRadius(150.f); // 🔴 半径设为 150，保证一定能碰到！
    
    // 强行用 C++ 设置碰撞，防止蓝图抽风
    Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 只和玩家(Pawn)发生重叠

    // 2. 初始化模型组件
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 模型不需要物理碰撞

    // 3. 初始化 UI 组件
    PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
    PickupWidget->SetupAttachment(RootComponent);
    PickupWidget->SetWidgetSpace(EWidgetSpace::Screen); // 🔴 Screen模式：UI永远面向屏幕
    PickupWidget->SetDrawSize(FVector2D(150.f, 50.f));
    PickupWidget->SetRelativeLocation(FVector(0.f, 0.f, 80.f)); // 把 UI 举到草的头顶
    PickupWidget->SetVisibility(false); // 默认隐藏

    // 4. 绑定重叠事件
    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItemPickup::OnOverlapBegin);
    Sphere->OnComponentEndOverlap.AddDynamic(this, &AItemPickup::OnOverlapEnd);
}

void AItemPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 检查是不是玩家碰到了草
    if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
    {
        // 显示 UI
        PickupWidget->SetVisibility(true);
        // 开启模型的高亮描边 (Stephen 教程里的后期处理描边深度值通常是 250)
        Mesh->SetRenderCustomDepth(true);
        Mesh->SetCustomDepthStencilValue(250); 
    }
}

void AItemPickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
    {
        // 玩家离开，隐藏 UI 和描边
        PickupWidget->SetVisibility(false);
        Mesh->SetRenderCustomDepth(false);
    }
}