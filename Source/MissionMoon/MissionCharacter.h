#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MissionCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCableComponent;
class USpringArmComponent;
class UCameraComponent;

UENUM(BlueprintType)
enum class ETetherState : uint8
{
	Inactive,
	Anchored,
	Reeling
};

UCLASS()
class MISSIONMOON_API AMissionCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMissionCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// -------------------------------------------------------------------------
	// Zero-G & Alignment Properties
	// -------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG|Physics")
	float GravityMagnitude = 980.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG|Alignment")
	float AlignmentInterpSpeed = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG|Trace")
	float SurfaceTraceDistance = 300.0f;

	// -------------------------------------------------------------------------
	// Gravity Tether Properties
	// -------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG|Tether")
	float TetherSpringConstant = 2600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG|Tether")
	float TetherDamping = 8.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG|Tether")
	float TetherMaxDistance = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG|Tether")
	float TangentialConservationRate = 0.94f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZeroG|Tether")
	ETetherState TetherState = ETetherState::Inactive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZeroG|Components")
	TObjectPtr<UCableComponent> TetherCable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZeroG|Components")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZeroG|Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	// -------------------------------------------------------------------------
	// Input Actions
	// -------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ZeroG|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ZeroG|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ZeroG|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ZeroG|Input")
	TObjectPtr<UInputAction> ShiftGravityAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ZeroG|Input")
	TObjectPtr<UInputAction> FireTetherAction;

	// -------------------------------------------------------------------------
	// Public Methods
	// -------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "ZeroG|Mechanics")
	void ShiftGravityToCrosshair();

	UFUNCTION(BlueprintCallable, Category = "ZeroG|Mechanics")
	void FireTether();

	UFUNCTION(BlueprintCallable, Category = "ZeroG|Mechanics")
	void ReleaseTether();

protected:
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void UpdateSurfaceAlignment(float DeltaTime);
	void ApplyCustomGravity(float DeltaTime);
	void UpdateTetherPhysics(float DeltaTime);

private:
	FVector CurrentGravityDirection;
	FQuat TargetOrientation;
	FVector TetherAnchorPoint;
	TWeakObjectPtr<UPrimitiveComponent> TetheredComponent;
};
