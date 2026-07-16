
#include "Components/ItemPickup.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/PlayerCharacter.h"

AItemPickup::AItemPickup()
{
    PrimaryActorTick.bCanEverTick = false;

    // Configure the root overlap sphere.
    Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
    RootComponent = Sphere;
    Sphere->InitSphereRadius(150.f);
    
    // Keep collision behavior independent of Blueprint defaults.
    Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // Configure the visual mesh.
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Configure the world interaction prompt.
    PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
    PickupWidget->SetupAttachment(RootComponent);
    PickupWidget->SetWidgetSpace(EWidgetSpace::Screen);
    PickupWidget->SetDrawSize(FVector2D(150.f, 50.f));
    PickupWidget->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
    PickupWidget->SetVisibility(false);

    // Bind overlap callbacks.
    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItemPickup::OnOverlapBegin);
    Sphere->OnComponentEndOverlap.AddDynamic(this, &AItemPickup::OnOverlapEnd);
}

void AItemPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Only players reveal the pickup prompt.
    if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
    {
        // Show the prompt and highlight.
        PickupWidget->SetVisibility(true);
        // Stencil value 250 selects the pickup outline.
        Mesh->SetRenderCustomDepth(true);
        Mesh->SetCustomDepthStencilValue(250); 
    }
}

void AItemPickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
    {
        // Hide the prompt and outline when the player leaves.
        PickupWidget->SetVisibility(false);
        Mesh->SetRenderCustomDepth(false);
    }
}
