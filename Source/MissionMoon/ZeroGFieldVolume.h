#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZeroGFieldVolume.generated.h"

class UBoxComponent;
class UPrimitiveComponent;

USTRUCT()
struct FPhysicsObjectOriginalState
{
	GENERATED_BODY()

	UPROPERTY()
	bool bWasGravityEnabled = true;

	UPROPERTY()
	float OriginalLinearDamping = 0.01f;

	UPROPERTY()
	float OriginalAngularDamping = 0.0f;
};

UCLASS()
class ANTIGRAVITYSHOWCASE_API AZeroGFieldVolume : public AActor
{
	GENERATED_BODY()

public:
	AZeroGFieldVolume();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Antigrav|Volume")
	TObjectPtr<UBoxComponent> VolumeBounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Antigrav|Physics")
	float ZeroGLinearDamping = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Antigrav|Physics")
	float ZeroGAngularDamping = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Antigrav|Physics")
	float InitialKickStrength = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Antigrav|Physics")
	float MicroTurbulenceStrength = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Antigrav|Physics")
	float TurbulenceFrequency = 0.005f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnComponentEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY()
	TMap<TWeakObjectPtr<UPrimitiveComponent>, FPhysicsObjectOriginalState> TrackedPhysicsBodies;
};
