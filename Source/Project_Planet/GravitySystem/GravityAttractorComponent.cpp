#include "GravityAttractorComponent.h"
#include "CustomGravityWorldSubSystem.h"

UGravityAttractorComponent::UGravityAttractorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;
}

void UGravityAttractorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGravityAttractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	BuildAsyncInput();
}

void UGravityAttractorComponent::OnRegister()
{
	Super::OnRegister();


	if (UWorld* World = GetWorld())
	{
		if (UCustomGravityWorldSubSystem* GravitySubsystem = World->GetSubsystem<UCustomGravityWorldSubSystem>())
		{
			GravitySubsystem->AddAttractor(this);
		}
	}
}

void UGravityAttractorComponent::OnUnregister()
{
	if (UWorld* World = GetWorld())
	{
		if (UCustomGravityWorldSubSystem* GravitySubsystem = World->GetSubsystem<UCustomGravityWorldSubSystem>())
		{
			GravitySubsystem->RemoveAttractor(this);
		}
	}

	Super::OnUnregister();
}

FGravityAttractorData UGravityAttractorComponent::GetGravityAttractorData() const
{
	FGravityAttractorData GravityAttractorData;
	GravityAttractorData.Location = GetComponentLocation();
	if (bUseGravityAtRadius)
	{
		GravityAttractorData.MassDotG = Gravity * Radius * Radius;
	}
	else
	{
		GravityAttractorData.MassDotG = Mass * 6.67430E-5;
	}
	return GravityAttractorData;
}

void UGravityAttractorComponent::BuildAsyncInput()
{
	if (ApplyGravity)
	{
		if (UWorld* World = GetWorld())
		{
			if (UCustomGravityWorldSubSystem* GravitySubsystem = World->GetSubsystem<UCustomGravityWorldSubSystem>())
			{
				GravitySubsystem->AddGravityAttractorData(GetGravityAttractorData());
			}
		}
	}
	else
	{
		if (UWorld* World = GetWorld())
		{
			if (UCustomGravityWorldSubSystem* GravitySubsystem = World->GetSubsystem<UCustomGravityWorldSubSystem>())
			{
				GravitySubsystem->RemoveGravityAttractorData(GetGravityAttractorData());
			}
		}
	}
}