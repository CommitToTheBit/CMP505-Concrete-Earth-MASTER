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
	A.productions.push_back([](LModuleType FModule) {
		LModuleType productionModule = FModule;
		productionModule.letter = "F";
		productionModule.staticRotation = 0.0f;
		return productionModule;
		});
	/*F.productions.push_back([](LModuleType FModule) {
		LModuleType productionModule = FModule;
		productionModule.letter = "+";
		productionModule.staticLength = 0.0f;
		productionModule.staticRotation = 90.0f*DirectX::XM_PI/180.0f;
		return productionModule;
		});
	F.productions.push_back([](LModuleType FModule) {
		LModuleType productionModule = FModule;
		productionModule.letter = "G";
		productionModule.staticRotation = 0.0f;
		return productionModule;
		});
	F.weight = 1.0f;
	AddProductionRule("F", F);*/

	ProductionRuleType G;
	G.productions.push_back([](LModuleType GModule) {
		LModuleType productionModule = GModule;
		productionModule.letter = "F";
		productionModule.staticRotation = 0.0f;
		return productionModule;
		});
	G.productions.push_back([](LModuleType GModule) {
		LModuleType productionModule = GModule;
		productionModule.letter = "-";
		productionModule.staticLength = 0.0f;
		productionModule.staticRotation = -90.0f*DirectX::XM_PI/180.0f;
		return productionModule;
		});
	G.productions.push_back([](LModuleType GModule) {
		LModuleType productionModule = GModule;
		productionModule.letter = "G";
		productionModule.staticRotation = 0.0f;
		return productionModule;
		});
	G.weight = 1.0f;
	AddProductionRule("G", G);

	// STEP 2: Write axiom...
	LModuleType FModule;
	FModule.letter = "F";
	FModule.period = 0.0f;
	FModule.aperiodicity = 0.0f;
	FModule.synchronisation = 0.0f;
	FModule.asynchronicity = 0.0f;
	FModule.staticLength = 1.0f;
	FModule.randomStaticLength = 0.0f;
	FModule.periodicLength = 0.0f;
	FModule.randomPeriodicLength = 0.0f;
	FModule.staticRotation = 0.0f;
	FModule.randomStaticRotation = 0.0f;
	FModule.periodicRotation = 0.0f;
	FModule.randomPeriodicRotation = 0.0f;
	FModule.staticWidth = width;
	FModule.randomStaticWidth = 0.0f;
	FModule.periodicWidth = 0.0f;
	FModule.randomPeriodicWidth = 0.0f;

	std::vector<LModuleType> axiom = std::vector<LModuleType>{ FModule };

	// STEP 3: Initialise...
	return __super::Initialize(device, axiom, iterations, seed, -iterations*DirectX::XM_PIDIV4); // NB: Oriented to best 'tend towards a limit'...
}
