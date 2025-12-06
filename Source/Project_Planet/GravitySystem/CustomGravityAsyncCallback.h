#include "Project_Planet/Global.h"

struct FGravityAttractorData
{
	FVector Location;
	double MassDotG;
	bool ApplyGravity;
};

struct PROJECT_PLANET_API FCustomGravityAsyncInput : public Chaos::FSimCallbackInput
{
	ECustomGravityState CurrentGravityState;
	FVector CurrentPlaneGravityDirection;

	TArray<FGravityAttractorData> GravityAttractorsData;

	void Reset()
	{
		GravityAttractorsData.Empty();
	}
};

class PROJECT_PLANET_API FCustomGravityAsyncCallback : public Chaos::TSimCallbackObject<
	FCustomGravityAsyncInput,
	Chaos::FSimCallbackNoOutput,
	Chaos::ESimCallbackOptions::PreIntegrate | Chaos::ESimCallbackOptions::Presimulate>
{
public:
	FCustomGravityAsyncCallback();
	virtual ~FCustomGravityAsyncCallback() override;

	virtual void OnPreSimulate_Internal() override;
	virtual void OnPreIntegrate_Internal() override;

protected:
	static double GravitationalConstant;
};