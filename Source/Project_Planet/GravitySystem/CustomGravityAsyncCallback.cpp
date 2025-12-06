#include "CustomGravityAsyncCallback.h"
#include "PBDRigidsSolver.h"
#include "Chaos/DebugDrawQueue.h"

using namespace Chaos;



FCustomGravityAsyncCallback::FCustomGravityAsyncCallback()
{
}

FCustomGravityAsyncCallback::~FCustomGravityAsyncCallback()
{
}

void FCustomGravityAsyncCallback::OnPreSimulate_Internal()
{
	// Here we don't do anything. We just had to register to this Callback to avoid having the "PreIntegrate" filtered.
	// It was a 5.6 workaround, and would not be needed for 5.7.
}

void FCustomGravityAsyncCallback::OnPreIntegrate_Internal()
{
	// We are running on the PT here... 
	if (FPBDRigidsSolver* MySolver = static_cast<FPBDRigidsSolver*>(GetSolver()))
	{
		// Get a reference to the PT input structure
		const FCustomGravityAsyncInput* Input = GetConsumerInput_Internal();
		if (Input)
		{
			// Iterate over all the currenly simulated rigid bodies - They are named Particles in Chaos. 
			TParticleView<FPBDRigidParticles> ActiveParticles = MySolver->GetParticles().GetNonDisabledDynamicView();

			for (auto& ActiveParticle : ActiveParticles)
			{
				if (ActiveParticle.Handle())
				{
					 //Draw current acceleration
					//FDebugDrawQueue::GetInstance().DrawDebugDirectionalArrow(ActiveParticle.GetX(), ActiveParticle.GetX() + ActiveParticle.Acceleration() , 20.f, FColor::Yellow, false, 0, 0, 1.f);

					FVector AdditionalAcceleration = FVector::ZeroVector;

					switch (Input->CurrentGravityState)
					{
						case ECustomGravityState::Sphere:

							// Compute the combined forces of all attractors
							for (auto& GravityAttractorData : Input->GravityAttractorsData)
							{
								// Direction
								FVector Direction(GravityAttractorData.Location - ActiveParticle.GetX());
								double SquaredDistance = FVector::DotProduct(Direction, Direction); // We'll be using UE units here, no meters... 
								Direction.Normalize();

								// Intensity
								double Intensity = GravityAttractorData.MassDotG / SquaredDistance;

								// Add the new acceleration to the force field.  
								AdditionalAcceleration += Intensity * Direction;

							}
							break;

						case ECustomGravityState::Plane:
						default:

							AdditionalAcceleration = Input->CurrentPlaneGravityDirection * PlaneGravityMagnitude * ParticleMassMultiplier;
							break;
					}


					// Add the force field value to the rigid body. 
					//ActiveParticle.SetAcceleration(ActiveParticle.Acceleration() + AdditionalAcceleration);
					ActiveParticle.AddForce(AdditionalAcceleration * ActiveParticle.M());
				}
			}
		}
	}
}