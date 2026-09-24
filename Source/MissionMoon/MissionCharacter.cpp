#include "MissionCharacter.h"
#include "MissionMoon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CableComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"

AMissionCharacter::AMissionCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	CurrentGravityDirection = FVector(0.0f, 0.0f, -1.0f);
	TargetOrientation = FQuat::Identity;

	// CMC Configuration for dynamic arbitrary gravity orientation
	UCharacterMovementComponent* CMC = GetCharacterMovement();
	CMC->GravityScale = 0.0f;
	CMC->bOrientRotationToMovement = false;
	CMC->MaxWalkSpeed = 650.0f;
	CMC->BrakingDecelerationWalking = 1200.0f;
	CMC->AirControl = 0.6f;

	// Camera Boom (Follows character with smooth lag)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 380.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 8.0f;

	// Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Visual Cable Component for Tether
	TetherCable = CreateDefaultSubobject<UCableComponent>(TEXT("TetherCable"));
	TetherCable->SetupAttachment(GetMesh());
	TetherCable->SetVisibility(false);
	TetherCable->CableLength = 50.0f;
	TetherCable->NumSegments = 12;
	TetherCable->SubstepTime = 0.015f;
	TetherCable->CableWidth = 2.5f;
}

void AMissionCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add Enhanced Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void AMissionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSurfaceAlignment(DeltaTime);
	ApplyCustomGravity(DeltaTime);
	UpdateTetherPhysics(DeltaTime);
}

void AMissionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMissionCharacter::Move);
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMissionCharacter::Look);
		}

		if (ShiftGravityAction)
		{
			EnhancedInputComponent->BindAction(ShiftGravityAction, ETriggerEvent::Started, this, &AMissionCharacter::ShiftGravityToCrosshair);
		}

		if (FireTetherAction)
		{
			EnhancedInputComponent->BindAction(FireTetherAction, ETriggerEvent::Started, this, &AMissionCharacter::FireTether);
			EnhancedInputComponent->BindAction(FireTetherAction, ETriggerEvent::Completed, this, &AMissionCharacter::ReleaseTether);
		}
	}
}

void AMissionCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Derive move direction relative to actor's current orientation
		const FVector ForwardDirection = GetActorForwardVector();
		const FVector RightDirection = GetActorRightVector();

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMissionCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMissionCharacter::UpdateSurfaceAlignment(float DeltaTime)
{
	const FVector ActorLoc = GetActorLocation();
	const FVector DownVector = -GetActorUpVector();
	const FVector TraceEnd = ActorLoc + (DownVector * SurfaceTraceDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const float SphereRadius = GetCapsuleComponent()->GetScaledCapsuleRadius() * 0.75f;
	const bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		ActorLoc,
		TraceEnd,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(SphereRadius),
		QueryParams
	);

	if (bHit && HitResult.ImpactNormal.IsNormalized())
	{
		const FVector NewUp = HitResult.ImpactNormal;
		CurrentGravityDirection = -NewUp;

		// Project current forward onto new tangent plane
		FVector Forward = GetActorForwardVector();
		FVector ProjectedForward = FVector::VectorPlaneProject(Forward, NewUp).GetSafeNormal();

		// Handle singularity (if looking straight into surface)
		if (ProjectedForward.IsNearlyZero())
		{
			ProjectedForward = FVector::VectorPlaneProject(GetActorRightVector(), NewUp).GetSafeNormal();
		}

		const FRotationMatrix TargetMatrix = FRotationMatrix::MakeFromZX(NewUp, ProjectedForward);
		TargetOrientation = TargetMatrix.ToQuat();
	}

	// Smoothly slerp without Euler gimbal lock
	const FQuat CurrentQuat = GetActorQuat();
	const FQuat SmoothedQuat = FQuat::Slerp(CurrentQuat, TargetOrientation, FMath::Clamp(DeltaTime * AlignmentInterpSpeed, 0.0f, 1.0f));

	SetActorRotation(SmoothedQuat);
}

void AMissionCharacter::ApplyCustomGravity(float DeltaTime)
{
	// Custom gravity acceleration along CurrentGravityDirection
	const FVector GravityAcceleration = CurrentGravityDirection * GravityMagnitude;
	GetCharacterMovement()->Velocity += GravityAcceleration * DeltaTime;
}

void AMissionCharacter::ShiftGravityToCrosshair()
{
	if (!FollowCamera) return;

	FVector CamLoc = FollowCamera->GetComponentLocation();
	FVector CamForward = FollowCamera->GetForwardVector();
	FVector TraceEnd = CamLoc + (CamForward * 8000.0f);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, CamLoc, TraceEnd, ECC_Visibility, QueryParams))
	{
		if (HitResult.ImpactNormal.IsNormalized())
		{
			const FVector NewUp = HitResult.ImpactNormal;
			CurrentGravityDirection = -NewUp;

			FVector Forward = GetActorForwardVector();
			FVector ProjectedForward = FVector::VectorPlaneProject(Forward, NewUp).GetSafeNormal();
			if (ProjectedForward.IsNearlyZero())
			{
				ProjectedForward = FVector::VectorPlaneProject(GetActorRightVector(), NewUp).GetSafeNormal();
			}

			TargetOrientation = FRotationMatrix::MakeFromZX(NewUp, ProjectedForward).ToQuat();

			// Initial gentle impulse towards the new surface
			GetCharacterMovement()->Velocity += CurrentGravityDirection * 350.0f;
		}
	}
}

void AMissionCharacter::FireTether()
{
	if (!FollowCamera) return;

	FVector CamLoc = FollowCamera->GetComponentLocation();
	FVector CamForward = FollowCamera->GetForwardVector();
	FVector TraceEnd = CamLoc + (CamForward * TetherMaxDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, CamLoc, TraceEnd, ECC_Visibility, QueryParams))
	{
		TetherAnchorPoint = HitResult.ImpactPoint;
		TetheredComponent = HitResult.GetComponent();
		TetherState = ETetherState::Anchored;

		if (TetherCable)
		{
			TetherCable->SetVisibility(true);
			TetherCable->SetWorldLocation(GetMesh()->GetSocketLocation(TEXT("hand_r")));
			TetherCable->EndLocation = TetherAnchorPoint;
		}
	}
}

void AMissionCharacter::ReleaseTether()
{
	TetherState = ETetherState::Inactive;
	TetheredComponent.Reset();

	if (TetherCable)
	{
		TetherCable->SetVisibility(false);
	}
}

void AMissionCharacter::UpdateTetherPhysics(float DeltaTime)
{
	if (TetherState != ETetherState::Anchored) return;

	// Update anchor if attached to a moving physics object
	if (TetheredComponent.IsValid() && TetheredComponent->IsSimulatingPhysics())
	{
		TetherAnchorPoint = TetheredComponent->GetComponentLocation();
	}

	const FVector PlayerLoc = GetActorLocation();
	const FVector TetherDelta = TetherAnchorPoint - PlayerLoc;
	const float Distance = TetherDelta.Size();

	if (Distance <= 140.0f)
	{
		ReleaseTether();
		return;
	}

	const FVector PullDirection = TetherDelta.GetSafeNormal();
	UCharacterMovementComponent* CMC = GetCharacterMovement();
	const FVector CurrentVel = CMC->Velocity;

	// Decompose velocity into radial and tangential components
	const float RadialSpeed = FVector::DotProduct(CurrentVel, PullDirection);
	const FVector RadialVel = RadialSpeed * PullDirection;
	const FVector TangentialVel = CurrentVel - RadialVel;

	// Critically damped spring-damper model
	const FVector SpringForce = PullDirection * TetherSpringConstant;
	const FVector DampingForce = -TetherDamping * RadialVel;
	const FVector TotalForce = SpringForce + DampingForce;

	// Conserve tangential inertia for orbital swinging
	const FVector AdjustedTangential = TangentialVel * FMath::Pow(TangentialConservationRate, DeltaTime);

	// Integrate
	CMC->Velocity = (RadialVel + (TotalForce * DeltaTime)) + AdjustedTangential;

	// Update cable visual endpoint
	if (TetherCable)
	{
		TetherCable->SetWorldLocation(GetMesh()->GetSocketLocation(TEXT("hand_r")));
		TetherCable->EndLocation = TetherAnchorPoint;
	}
}
