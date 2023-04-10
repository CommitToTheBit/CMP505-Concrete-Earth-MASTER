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
	ProductionRuleType FClockwise;
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "";
		productionModule.staticLength = FClockwiseModule.staticLength/4.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "FAntiClockwise";
		productionModule.staticLength = FClockwiseModule.staticLength/2.0f;
		productionModule.staticRotation = 0.0f; // NB: Needed on all rules but the first!
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "";
		productionModule.staticLength = FClockwiseModule.staticLength/4.0f;
		productionModule.staticRotation = 0.0f;
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
		productionModule.letter = "";
		productionModule.staticLength = FClockwiseModule.staticLength/4.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "FClockwise";
		productionModule.staticLength = FClockwiseModule.staticLength/2.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "";
		productionModule.staticLength = FClockwiseModule.staticLength/4.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "";
		productionModule.staticLength = FClockwiseModule.staticLength/2.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "FAntiClockwise";
		productionModule.staticLength = FClockwiseModule.staticLength/2.0f;
		productionModule.staticRotation = -DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "";
		productionModule.staticLength = FClockwiseModule.staticLength/2.0f;
		productionModule.staticRotation = DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType FClockwiseModule) {
		LModuleType productionModule = FClockwiseModule;
		productionModule.letter = "";
		productionModule.staticLength = FClockwiseModule.staticLength;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.weight = 1.0f;
	AddProductionRule("FClockwise", FClockwise);

	ProductionRuleType FAntiClockwise;
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "";
		productionModule.staticLength = FAntiClockwiseModule.staticLength;
		return productionModule;
	});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/2.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
	});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "FClockwise";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/2.0f;
		productionModule.staticRotation = DirectX::XM_PI/3.0f;
		return productionModule;
	});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/2.0f;
		productionModule.staticRotation = -DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/4.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "FAntiClockwise";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/2.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
	});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/4.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
	});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "FClockwise";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/2.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
	});
	FAntiClockwise.productions.push_back([](LModuleType FAntiClockwiseModule) {
		LModuleType productionModule = FAntiClockwiseModule;
		productionModule.letter = "";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/4.0f;
		productionModule.staticRotation = 0.0f;
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
		productionModule.letter = "";
		productionModule.staticLength = FAntiClockwiseModule.staticLength/4.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
	});
	FAntiClockwise.weight = 1.0f;
	AddProductionRule("FAntiClockwise", FAntiClockwise);

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

	LModuleType FClockwiseModule = BaseModule;
	//FClockwiseModule.letter = "FClockwise";
	FClockwiseModule.staticLength = 2.0f;

	LModuleType FAntiClockwiseModule = BaseModule;
	FAntiClockwiseModule.letter = "FAntiClockwise";
	FAntiClockwiseModule.staticLength = 2.0f;

	LModuleType XModule = BaseModule;
	XModule.staticLength = 1.0f;

	LModuleType YModule = BaseModule;
	YModule.staticLength = 2.0f;

	LModuleType AntiClockwise120Module = BaseModule;
	AntiClockwise120Module.staticRotation = 2.0f*DirectX::XM_PI/3.0f;

	LModuleType AntiClockwise60Module = BaseModule;
	AntiClockwise60Module.staticRotation = DirectX::XM_PI/3.0f;

	LModuleType Clockwise60Module = BaseModule;
	Clockwise60Module.staticRotation = -DirectX::XM_PI/3.0f;

	std::vector<LModuleType> axiom = std::vector<LModuleType>{ 
		/*FClockwiseModule,
		XModule, 
		FClockwiseModule,
		XModule, 
		AntiClockwise120Module, 
		YModule, 
		AntiClockwise60Module, 
		FClockwiseModule,
		Clockwise60Module, 
		YModule, 
		AntiClockwise120Module, 
		XModule,*/
		FAntiClockwiseModule,
		/*XModule,
		AntiClockwise120Module, 
		YModule*/
	};

	// STEP 3: Initialise...
	return __super::Initialize(device, axiom, iterations, seed, 0.0f); // NB: Oriented to best 'tend towards a limit'...
}
