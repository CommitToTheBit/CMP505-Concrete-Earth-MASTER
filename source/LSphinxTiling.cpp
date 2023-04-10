#include "pch.h"
#include "LSphinxTiling.h"

LSphinxTiling::LSphinxTiling()
{

}


LSphinxTiling::~LSphinxTiling()
{

}

bool LSphinxTiling::Initialize(ID3D11Device* device, float width, int iterations, float seed)
{
	// STEP 1: Write production rules...
	ProductionRuleType F;
	F.productions.push_back([](LModuleType FModule) {
		LModuleType productionModule = FModule;
		productionModule.letter = "X";
		return productionModule;
		});
	F.productions.push_back([](LModuleType FModule) {
		LModuleType productionModule = FModule;
		productionModule.letter = "[";
		return productionModule;
		});
	F.productions.push_back([](LModuleType FModule) {
		LModuleType productionModule = FModule;
		productionModule.letter = "F";
		productionModule.staticLength = FModule.staticLength/2.0f;
		productionModule.staticRotation = -2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	F.productions.push_back([](LModuleType FModule) {
		LModuleType productionModule = FModule;
		productionModule.letter = "G";
		productionModule.staticLength = FModule.staticLength/2.0f;
		productionModule.staticRotation = DirectX::XM_PI/3.0f;
		return productionModule;
		});
	F.productions.push_back([](LModuleType FModule) {
		LModuleType productionModule = FModule;
		productionModule.letter = "Y";
		productionModule.staticLength = FModule.staticLength/2.0f;
		productionModule.staticRotation = -DirectX::XM_PI/3.0f;
		return productionModule;
		});
	F.productions.push_back([](LModuleType FModule) {
		LModuleType productionModule = FModule;
		productionModule.letter = "]";
		return productionModule;
		});
	F.weight = 1.0f;
	AddProductionRule("F", F);

	ProductionRuleType G;
	G.productions.push_back([](LModuleType GModule) {
		LModuleType productionModule = GModule;
		productionModule.letter = "Z";
		return productionModule;
		});
	G.productions.push_back([](LModuleType GModule) {
		LModuleType productionModule = GModule;
		productionModule.letter = "[";
		return productionModule;
		});
	G.productions.push_back([](LModuleType GModule) {
		LModuleType productionModule = GModule;
		productionModule.letter = "Y";
		productionModule.staticLength = GModule.staticLength/2.0f;
		productionModule.staticRotation = -2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	G.productions.push_back([](LModuleType GModule) {
		LModuleType productionModule = GModule;
		productionModule.letter = "G";
		productionModule.staticLength = GModule.staticLength/2.0f;
		productionModule.staticRotation = -DirectX::XM_PI/3.0f;
		return productionModule;
		});
	G.productions.push_back([](LModuleType GModule) {
		LModuleType productionModule = GModule;
		productionModule.letter = "Y";
		productionModule.staticLength = FModule.staticLength/2.0f;
		productionModule.staticRotation = -DirectX::XM_PI/3.0f;
		return productionModule;
		});
	G.productions.push_back([](LModuleType GModule) {
		LModuleType productionModule = GModule;
		productionModule.letter = "]";
		return productionModule;
		});
	G.weight = 1.0f;
	AddProductionRule("G", G);

	// STEP 2: Write axiom...
	// FIXME: Add constructor...
	LModuleType BaseModule;
	BaseModule.letter = "";
	BaseModule.period = 0.0f;
	BaseModule.aperiodicity = 0.0f;
	BaseModule.synchronisation = 0.0f;
	BaseModule.asynchronicity = 0.0f;
	BaseModule.staticLength = 0.0f;
	BaseModule.randomStaticLength = 0.0f;
	BaseModule.periodicLength = 0.0f;
	BaseModule.randomPeriodicLength = 0.0f;
	BaseModule.staticRotation = 0.0f;
	BaseModule.randomStaticRotation = 0.0f;
	BaseModule.periodicRotation = 0.0f;
	BaseModule.randomPeriodicRotation = 0.0f;
	BaseModule.staticWidth = width;
	BaseModule.randomStaticWidth = 0.0f;
	BaseModule.periodicWidth = 0.0f;
	BaseModule.randomPeriodicWidth = 0.0f;

	LModuleType XModule = BaseModule;
	XModule.letter = "X";
	XModule.staticLength = 2.0f;
	XModule.staticRotation = DirectX::XM_PI/3.0f;

	LModuleType FModule = BaseModule;
	FModule.letter = "F";
	FModule.staticLength = 1.0f;
	FModule.staticRotation = -2.0f*DirectX::XM_PI/3.0f;

	LModuleType GModule = BaseModule;
	GModule.letter = "G";
	GModule.staticLength = 1.0f;
	GModule.staticRotation = DirectX::XM_PI/3.0f;

	LModuleType YModule = BaseModule;
	YModule.letter = "Y";
	YModule.staticLength = 1.0f;
	YModule.staticRotation = -DirectX::XM_PI/3.0f;

	LModuleType ZModule = BaseModule;
	ZModule.letter = "Z";
	ZModule.staticLength = 3.0f;
	ZModule.staticRotation = -2.0f*DirectX::XM_PI/3.0f;

	LModuleType LoopModule = XModule;
	LoopModule.staticRotation = -2.0f*DirectX::XM_PI/3.0f;

	std::vector<LModuleType> axiom = std::vector<LModuleType>{ XModule, FModule, GModule, YModule, ZModule, LoopModule };

	// STEP 3: Initialise...
	return __super::Initialize(device, axiom, iterations, seed, 0.0f); // NB: Oriented to best 'tend towards a limit'...
}
