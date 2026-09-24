#include "ZeroGFieldVolume.h"
#include "MissionMoon.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/KismetMathLibrary.h"

AZeroGFieldVolume::AZeroGFieldVolume()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	VolumeBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("VolumeBounds"));
	RootComponent = VolumeBounds;
	VolumeBounds->SetBoxExtent(FVector(1500.0f, 1500.0f, 800.0f));
	VolumeBounds->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	VolumeBounds->SetGenerateOverlapEvents(true);
}

void AZeroGFieldVolume::BeginPlay()
{
	Super::BeginPlay();

	VolumeBounds->OnComponentBeginOverlap.AddDynamic(this, &AZeroGFieldVolume::OnComponentEnter);
	VolumeBounds->OnComponentEndOverlap.AddDynamic(this, &AZeroGFieldVolume::OnComponentExit);
}

void AZeroGFieldVolume::OnComponentEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherComp || !OtherComp->IsSimulatingPhysics()) return;

	TWeakObjectPtr<UPrimitiveComponent> WeakComp = OtherComp;
	if (TrackedPhysicsBodies.Contains(WeakComp)) return;

	// Cache original state for clean exit restoration
	FPhysicsObjectOriginalState OriginalState;
	OriginalState.bWasGravityEnabled = OtherComp->IsGravityEnabled();
	OriginalState.OriginalLinearDamping = OtherComp->GetLinearDamping();
	OriginalState.OriginalAngularDamping = OtherComp->GetAngularDamping();
	TrackedPhysicsBodies.Add(WeakComp, OriginalState);

	// Apply Zero-G viscous physics
	OtherComp->SetEnableGravity(false);
	OtherComp->SetLinearDamping(ZeroGLinearDamping);
	OtherComp->SetAngularDamping(ZeroGAngularDamping);

	// Apply off-center impulse scaled by object mass to generate slow, heavy rotational momentum
	const float Mass = OtherComp->GetMass();
	const FVector RandomHorizontal = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FVector::UpVector, 30.0f);
	const FVector Impulse = (RandomHorizontal * (InitialKickStrength * 0.6f) + FVector(0.0f, 0.0f, InitialKickStrength)) * Mass;

	const FVector CoM = OtherComp->GetCenterOfMass();
	const FVector TorqueOffset = UKismetMathLibrary::RandomUnitVector() * 30.0f;

	OtherComp->AddImpulseAtLocation(Impulse, CoM + TorqueOffset);
}

void AZeroGFieldVolume::OnComponentExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherComp) return;

	TWeakObjectPtr<UPrimitiveComponent> WeakComp = OtherComp;
	if (FPhysicsObjectOriginalState* State = TrackedPhysicsBodies.Find(WeakComp))
	{
		OtherComp->SetEnableGravity(State->bWasGravityEnabled);
		OtherComp->SetLinearDamping(State->OriginalLinearDamping);
		OtherComp->SetAngularDamping(State->OriginalAngularDamping);
		TrackedPhysicsBodies.Remove(WeakComp);
	}
}

void AZeroGFieldVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float Time = GetWorld()->GetTimeSeconds();

	// Clean up stale pointers and apply Perlin 3D turbulence forces
	for (auto It = TrackedPhysicsBodies.CreateIterator(); It; ++It)
	{
		TWeakObjectPtr<UPrimitiveComponent> WeakComp = It.Key();
		if (!WeakComp.IsValid() || !WeakComp->IsSimulatingPhysics())
		{
			It.RemoveCurrent();
			continue;
		}

		UPrimitiveComponent* Prim = WeakComp.Get();
		const FVector Pos = Prim->GetComponentLocation();

		// Sample continuous 3D Perlin space
		const float NoiseX = FMath::PerlinNoise3D(FVector(Pos.X * TurbulenceFrequency, Pos.Y * TurbulenceFrequency, Time * 0.2f));
		const float NoiseY = FMath::PerlinNoise3D(FVector(Pos.Y * TurbulenceFrequency, Pos.Z * TurbulenceFrequency, Time * 0.2f));
		const float NoiseZ = FMath::PerlinNoise3D(FVector(Pos.Z * TurbulenceFrequency, Pos.X * TurbulenceFrequency, Time * 0.15f));

		const FVector MicroForce = FVector(NoiseX, NoiseY, NoiseZ * 1.4f) * Prim->GetMass() * MicroTurbulenceStrength;
		Prim->AddForce(MicroForce);
	}
}
