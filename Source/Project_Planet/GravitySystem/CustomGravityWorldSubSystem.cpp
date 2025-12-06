#include "CustomGravityWorldSubSystem.h"
#include "PBDRigidsSolver.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "EngineUtils.h"

void UCustomGravityWorldSubSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

	UpdateCMCGravities();
	ChangeCurrentGravityState(CurrentGravityState, CurrentPlaneGravityDirection);
}

TStatId UCustomGravityWorldSubSystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UCustomGravityWorldSubSystem, STATGROUP_Tickables);
}

void UCustomGravityWorldSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

	if (UWorld* World = GetWorld())
	{
		ActorSpawnedHandle = World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &UCustomGravityWorldSubSystem::AddActorToTrackedCharacters));
		ActorDestroyedHandle = World->AddOnActorDestroyedHandler(FOnActorDestroyed::FDelegate::CreateUObject(this, &UCustomGravityWorldSubSystem::RemoveActorFromTrackedCharacters));
	}
}

void UCustomGravityWorldSubSystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		World->RemoveOnActorSpawnedHandler(ActorSpawnedHandle);
		World->RemoveOnActorDestroyedHandler(ActorDestroyedHandle);
	}

	TrackedCharacterMovementComponents.Empty();

    Super::Deinitialize();
}

void UCustomGravityWorldSubSystem::OnWorldBeginPlay(UWorld& World)
{
    Super::OnWorldBeginPlay(World);

	//GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, *FString::Printf(TEXT("%d Attractors in the World"), Attractors.Num()));

	if (!IsAsyncCallbackRegistered())
	{
		RegisterAsyncCallback();
	}

	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AddActorToTrackedCharacters(*ActorItr);
	}

	ChangeCurrentGravityState(CurrentGravityState, CurrentPlaneGravityDirection);
}

void UCustomGravityWorldSubSystem::UpdateCMCGravities()
{
	for (auto CMComponent : TrackedCharacterMovementComponents)
	{
		FVector AdditionalAcceleration = FVector::ZeroVector;

		switch (CurrentGravityState)
		{
		case ECustomGravityState::Sphere:

			for (auto& GravityAttractor : Attractors)
			{
				if (GravityAttractor->ApplyGravity)
				{
					FGravityAttractorData GravityAttractorData = GravityAttractor->GetGravityAttractorData();

					FVector Direction(GravityAttractorData.Location - CMComponent->GetActorLocation());
					double SquaredDistance = FVector::DotProduct(Direction, Direction);
					Direction.Normalize();

					double Intensity = GravityAttractorData.MassDotG / SquaredDistance;

					AdditionalAcceleration += Intensity * Direction;
				}
			}

			break;
		case ECustomGravityState::Plane:
		default:

			AdditionalAcceleration = CurrentPlaneGravityDirection * PlaneGravityMagnitude;

			break;
		}


		//DrawDebugDirectionalArrow(GetWorld(), CMComponent->GetActorLocation(), CMComponent->GetActorLocation() + AdditionalAcceleration, 1.0, FColor::Red, 0, false, 1.0f  );
		//DrawDebugString(GetWorld(), CMComponent->GetActorLocation(), * FString::Printf(TEXT("%.2f"), AdditionalAcceleration.Length()), nullptr, FColor::Red, 0, false, 1.0f  );

		CMComponent->AddForce(AdditionalAcceleration * CMComponent->Mass);
		CMComponent->SetGravityDirection(AdditionalAcceleration.GetSafeNormal());
	}
}

void UCustomGravityWorldSubSystem::AddAttractor(UGravityAttractorComponent* GravityAttractorComponent)
{
    Attractors.Add(GravityAttractorComponent);
}

void UCustomGravityWorldSubSystem::RemoveAttractor(UGravityAttractorComponent* GravityAttractorComponent)
{
    Attractors.Remove(GravityAttractorComponent);
}

void UCustomGravityWorldSubSystem::RegisterAsyncCallback()
{
	if (UWorld* World = GetWorld())
	{
		if (FPhysScene* PhysScene = World->GetPhysicsScene())
		{
			AsyncCallback = PhysScene->GetSolver()->CreateAndRegisterSimCallbackObject_External<FCustomGravityAsyncCallback>();
		}
	}
}

bool UCustomGravityWorldSubSystem::IsAsyncCallbackRegistered() const
{
	return AsyncCallback != nullptr;
}

void UCustomGravityWorldSubSystem::AddGravityAttractorData(const FGravityAttractorData& InputData) const
{
	if (IsAsyncCallbackRegistered())
	{
		FCustomGravityAsyncInput* Input = AsyncCallback->GetProducerInputData_External();
		Input->GravityAttractorsData.Add(InputData);
	}
}

//Self Add
void UCustomGravityWorldSubSystem::RemoveGravityAttractorData(const FGravityAttractorData& InputData) const
{
	if (IsAsyncCallbackRegistered())
	{
		FCustomGravityAsyncInput* Input = AsyncCallback->GetProducerInputData_External();
		//TODO
		//Input->GravityAttractorsData.Remove(InputData);
	}
}

void UCustomGravityWorldSubSystem::AddActorToTrackedCharacters(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	Actor->ForEachComponent<UCharacterMovementComponent>(true, [this](UCharacterMovementComponent* CMComponent)
		{
			if (CMComponent)
			{
				TrackedCharacterMovementComponents.Add(CMComponent);
			}
		});
}

void UCustomGravityWorldSubSystem::RemoveActorFromTrackedCharacters(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	Actor->ForEachComponent<UCharacterMovementComponent>(true, [this](UCharacterMovementComponent* CMComponent)
		{
			if (CMComponent)
			{
				TrackedCharacterMovementComponents.Remove(CMComponent);
			}
		});
}

void UCustomGravityWorldSubSystem::ChangeCurrentGravityState(ECustomGravityState NewState, FVector NewPlaneGravityDirection)
{
	CurrentGravityState = NewState;
	CurrentPlaneGravityDirection = NewPlaneGravityDirection;

	if (IsAsyncCallbackRegistered())
	{
		FCustomGravityAsyncInput* Input = AsyncCallback->GetProducerInputData_External();
		Input->CurrentGravityState = NewState;
		Input->CurrentPlaneGravityDirection = NewPlaneGravityDirection;
	}
}