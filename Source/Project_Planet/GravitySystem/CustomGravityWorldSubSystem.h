#pragma once

#include "Project_Planet/Global.h"
#include "Subsystems/WorldSubsystem.h"
#include "GravityAttractorComponent.h"
#include "CustomGravityAsyncCallback.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomGravityWorldSubSystem.generated.h"

UCLASS()
class PROJECT_PLANET_API UCustomGravityWorldSubSystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void OnWorldBeginPlay(UWorld& World) override;

	void AddAttractor(UGravityAttractorComponent* GravityAttractorComponent);
	void RemoveAttractor(UGravityAttractorComponent* GravityAttractorComponent);

	virtual void RegisterAsyncCallback();
	virtual bool IsAsyncCallbackRegistered() const;
	void AddGravityAttractorData(const FGravityAttractorData& InputData) const;
	void RemoveGravityAttractorData(const FGravityAttractorData& InputData) const;
	FCustomGravityAsyncCallback* AsyncCallback = nullptr;

	UFUNCTION(BlueprintCallable, Category = "CustomGravitySystem")
	void ChangeCurrentGravityState(ECustomGravityState NewState, FVector NewPlaneGravityDirection = FVector(0,0,-1));

protected:
	TArray<TWeakObjectPtr<UGravityAttractorComponent>> Attractors;
	TArray<TWeakObjectPtr<UCharacterMovementComponent>> TrackedCharacterMovementComponents;

	void AddActorToTrackedCharacters(AActor* Actor);
	void RemoveActorFromTrackedCharacters(AActor* Actor);

	FDelegateHandle ActorSpawnedHandle;
	FDelegateHandle ActorDestroyedHandle;

	void UpdateCMCGravities();
	//void UpdateParticlesGravities();

	ECustomGravityState CurrentGravityState = ECustomGravityState::Plane;
	FVector CurrentPlaneGravityDirection = FVector::DownVector;
};
