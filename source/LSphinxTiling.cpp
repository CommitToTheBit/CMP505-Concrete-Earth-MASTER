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
	ProductionRuleType FAntiClockwise;
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "FClockwise";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/2.0f;
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "[";
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "HClockwise";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/3.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "X";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/6.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "GClockwise";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/6.0f;
		productionModule.staticRotation = -DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "~X";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/6.0f;
		productionModule.staticRotation = DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "]";
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "FClockwise";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/2.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "HClockwise";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/3.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "[";
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "X";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/6.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "GClockwise";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/6.0f;
		productionModule.staticRotation = -DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "~X";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/6.0f;
		productionModule.staticRotation = DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "]";
		return productionModule;
		});
	FAntiClockwise.weight = 1.0f;
	AddProductionRule("FAntiClockwise", FAntiClockwise);

	ProductionRuleType FClockwise;
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "FAntiClockwise";
		productionModule.staticLength = FClockwiseModule.staticLength/2.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "[";
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "~X";
		productionModule.staticLength = FClockwiseModule.staticLength/6.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "GAntiClockwise";
		productionModule.staticLength = FClockwiseModule.staticLength/6.0f;
		productionModule.staticRotation = DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "X";
		productionModule.staticLength = FClockwiseModule.staticLength/6.0f;
		productionModule.staticRotation = -DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "HAntiClockwise";
		productionModule.staticLength = FClockwiseModule.staticLength/3.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "]";
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "FAntiClockwise";
		productionModule.staticLength = FClockwiseModule.staticLength/2.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "[";
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "~X";
		productionModule.staticLength = FClockwiseModule.staticLength/6.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "GAntiClockwise";
		productionModule.staticLength = FClockwiseModule.staticLength/6.0f;
		productionModule.staticRotation = DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "X";
		productionModule.staticLength = FClockwiseModule.staticLength/6.0f;
		productionModule.staticRotation = -DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "HAntiClockwise";
		productionModule.staticLength = FClockwiseModule.staticLength/3.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "]";
		return productionModule;
		});
	FClockwise.weight = 1.0f;
	AddProductionRule("FClockwise", FClockwise);

	ProductionRuleType GAntiClockwise;
	GAntiClockwise.productions.push_back([](LModuleType GAntiClockwiseModule) {
		LModuleType productionModule = GAntiClockwiseModule;
		productionModule.letter = "[";
		return productionModule;
		});
	GAntiClockwise.productions.push_back([](LModuleType GAntiClockwiseModule) {
		LModuleType productionModule = GAntiClockwiseModule;
		productionModule.letter = "FClockwise";
		productionModule.staticLength = 3.0f*GAntiClockwiseModule.staticLength/2.0f;
		return productionModule;
		});
	GAntiClockwise.productions.push_back([](LModuleType GAntiClockwiseModule) {
		LModuleType productionModule = GAntiClockwiseModule;
		productionModule.letter = "]";
		return productionModule;
		});
	GAntiClockwise.productions.push_back([](LModuleType GAntiClockwiseModule) {
		LModuleType productionModule = GAntiClockwiseModule;
		productionModule.letter = "X";
		return productionModule;
		});
	GAntiClockwise.weight = 1.0f;
	AddProductionRule("GAntiClockwise", GAntiClockwise);

	ProductionRuleType GClockwise;
	GClockwise.productions.push_back([](LModuleType GClockwiseModule) {
		LModuleType productionModule = GClockwiseModule;
		productionModule.letter = "X";	
		productionModule.staticLength = GClockwiseModule.staticLength/2.0f;
		productionModule.staticRotation = GClockwiseModule.staticRotation-DirectX::XM_PI; // NB: Go back on self, then forward!
		return productionModule;
		});
	GClockwise.productions.push_back([](LModuleType GClockwiseModule) {
		LModuleType productionModule = GClockwiseModule;
		productionModule.letter = "FAntiClockwise";
		productionModule.staticLength = 3.0f*GClockwiseModule.staticLength/2.0f;
		productionModule.staticRotation = DirectX::XM_PI;
		return productionModule;
		});

	GClockwise.weight = 1.0f;
	AddProductionRule("GClockwise", GClockwise);

	ProductionRuleType HAntiClockwise;
	HAntiClockwise.productions.push_back([](LModuleType HAntiClockwiseModule) {
		LModuleType productionModule = HAntiClockwiseModule;
		productionModule.letter = "FAntiClockwise";
		productionModule.staticLength = 3.0f*HAntiClockwiseModule.staticLength/4.0f;
		return productionModule;
		});
	HAntiClockwise.weight = 1.0f;
	AddProductionRule("HAntiClockwise", HAntiClockwise);

	ProductionRuleType HClockwise;
	HClockwise.productions.push_back([](LModuleType HClockwiseModule) {
		LModuleType productionModule = HClockwiseModule;
		productionModule.letter = "FClockwise";
		productionModule.staticLength = 3.0f*HClockwiseModule.staticLength/4.0f;
		return productionModule;
		});
	HClockwise.productions.push_back([](LModuleType HClockwiseModule) {
		LModuleType productionModule = HClockwiseModule;
		productionModule.letter = "X";
		productionModule.staticLength = HClockwiseModule.staticLength/4.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
		});
	HClockwise.weight = 1.0f;
	AddProductionRule("HClockwise", HClockwise);

	ProductionRuleType X;
	X.weight = 1.0f;
	AddProductionRule("~X", X);

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

	LModuleType FAntiClockwiseModule = BaseModule;
	FAntiClockwiseModule.letter = "FAntiClockwise";
	FAntiClockwiseModule.staticLength = 3.0f;
	FAntiClockwiseModule.staticRotation = 0.0f;

	LModuleType XModule1 = BaseModule;
	XModule1.letter = "~X";
	XModule1.staticLength = 1.0f;
	XModule1.staticRotation = 2.0f*DirectX::XM_PI/3.0f;

	LModuleType GAntiClockwiseModule = BaseModule;
	GAntiClockwiseModule.letter = "GAntiClockwise";
	GAntiClockwiseModule.staticLength = 1.0f;
	GAntiClockwiseModule.staticRotation = DirectX::XM_PI/3.0f;

	LModuleType XModule2 = BaseModule;
	XModule2.letter = "Y";
	XModule2.staticLength = 1.0f;
	XModule2.staticRotation = -DirectX::XM_PI/3.0f;

	LModuleType HAntiClockwiseModule = BaseModule;
	HAntiClockwiseModule.letter = "HAntiClockwise";
	HAntiClockwiseModule.staticLength = 2.0f;
	HAntiClockwiseModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;

	std::vector<LModuleType> axiom = std::vector<LModuleType>{ FAntiClockwiseModule , XModule1, GAntiClockwiseModule, XModule2, HAntiClockwiseModule };

	// STEP 3: Initialise...
	return __super::Initialize(device, axiom, iterations, seed, 0.0f); // NB: Oriented to best 'tend towards a limit'...
}
