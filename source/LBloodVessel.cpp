#include "pch.h"
#include "LBloodVessel.h"

LBloodVessel::LBloodVessel()
{

}


LBloodVessel::~LBloodVessel()
{

}

bool LBloodVessel::Initialize(ID3D11Device* device, float width, int iterations, float seed)
{
	// STEP 1: Write production rules...
	ProductionRuleType A;
	A.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "B";
		return productionModule;
		});
	A.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "[";
		return productionModule;
		});
	A.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.staticRotation = 30.0f*DirectX::XM_PI/180.0f;
		productionModule.randomStaticRotation = 2.0f*LModule.randomStaticRotation;
		return productionModule;
		});
	A.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "]";
		return productionModule;
		});
	A.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.staticRotation = -30.0f*DirectX::XM_PI/180.0f;
		productionModule.randomStaticRotation = 2.0f*LModule.randomStaticRotation;
		return productionModule;
		});
	A.weight = 1.0f;
	AddProductionRule("A", A);

	ProductionRuleType B;
	B.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "B";
		return productionModule;
		});
	B.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "B";
		productionModule.staticRotation = 0.0f;
		return productionModule;
		});
	B.weight = 1.0f;
	AddProductionRule("B", B);

	// STEP 2: Write axiom...
	LModuleType AModule;
	AModule.letter = "A";
	AModule.period = 10.0f;
	AModule.aperiodicity = 10.0f;
	AModule.synchronisation = 0.0f;
	AModule.asynchronicity = 1.0f;
	AModule.staticLength = 1.0f;
	AModule.randomStaticLength = 0.5f;
	AModule.staticRotation = 0.0f;
	AModule.randomStaticRotation = (15.0f/pow(2.0f,iterations))*DirectX::XM_PI/180.0f;
	AModule.randomPeriodicRotation = (15.0f/pow(2.0f, iterations))*DirectX::XM_PI/180.0f;
	AModule.staticWidth = width;

	std::vector<LModuleType> axiom = std::vector<LModuleType>{ AModule };

	// STEP 3: Initialise...
	return __super::Initialize(device, axiom, iterations, seed, DirectX::XM_PIDIV2); // NB: Oriented to best 'tend towards a limit'...
}
