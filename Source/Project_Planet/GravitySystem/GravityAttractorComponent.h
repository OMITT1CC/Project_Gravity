#pragma once

#include "Project_Planet/Global.h"
#include "Components/SceneComponent.h"
#include "GravityAttractorComponent.generated.h"

struct FGravityAttractorData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_PLANET_API UGravityAttractorComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGravityAttractorComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attractor")
	bool bUseGravityAtRadius = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attractor", meta = (ForceUnits="Kg", ClampMin = "1", EditConditionHides, EditCondition = "!bUseGravityAtRadius"))
	double Mass = 5.9722E24;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attractor", meta = (EditConditionHides, EditCondition = "bUseGravityAtRadius"))
	double Gravity = 981.0;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attractor", meta = (EditConditionHides, EditCondition = "bUseGravityAtRadius"))
	double Radius = 5000.0;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attractor")
	bool ApplyGravity = true;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;	

public:
	FGravityAttractorData GetGravityAttractorData() const;

private:
	virtual void BuildAsyncInput();
};
