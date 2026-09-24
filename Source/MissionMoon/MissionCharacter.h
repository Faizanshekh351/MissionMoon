#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AntigravCharacter.generated.h"

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
class ANTIGRAVITYSHOWCASE_API AAntigravCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAntigravCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// -------------------------------------------------------------------------
	// Antigravity & Alignment Properties
	// -------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Antigrav|Physics")
	float GravityMagnitude = 980.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Antigrav|Alignment")
	float AlignmentInterpSpeed = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Antigrav|Trace")
	float SurfaceTraceDistance = 300.0f;

	// -------------------------------------------------------------------------
	// Gravity Tether Properties
	// -------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Antigrav|Tether")
	float TetherSpringConstant = 2600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Antigrav|Tether")
	float TetherDamping = 8.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Antigrav|Tether")
	float TetherMaxDistance = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Antigrav|Tether")
	float TangentialConservationRate = 0.94f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Antigrav|Tether")
	ETetherState TetherState = ETetherState::Inactive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Antigrav|Components")
	TObjectPtr<UCableComponent> TetherCable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Antigrav|Components")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Antigrav|Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	// -------------------------------------------------------------------------
	// Input Actions
	// -------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Antigrav|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Antigrav|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Antigrav|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Antigrav|Input")
	TObjectPtr<UInputAction> ShiftGravityAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Antigrav|Input")
	TObjectPtr<UInputAction> FireTetherAction;

	// -------------------------------------------------------------------------
	// Public Methods
	// -------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Antigrav|Mechanics")
	void ShiftGravityToCrosshair();

	UFUNCTION(BlueprintCallable, Category = "Antigrav|Mechanics")
	void FireTether();

	UFUNCTION(BlueprintCallable, Category = "Antigrav|Mechanics")
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
