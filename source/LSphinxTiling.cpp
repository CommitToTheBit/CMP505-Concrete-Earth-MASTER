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
	FClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "";
		productionModule.staticLength = LModule.staticLength/4.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "FAntiClockwise";
		productionModule.staticLength = LModule.staticLength/2.0f;
		productionModule.staticRotation = 0.0f; // NB: Needed on all rules but the first!
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "";
		productionModule.staticLength = LModule.staticLength/4.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "FAntiClockwise";
		productionModule.staticLength = LModule.staticLength/2.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "";
		productionModule.staticLength = LModule.staticLength/4.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "FClockwise";
		productionModule.staticLength = LModule.staticLength/2.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "";
		productionModule.staticLength = LModule.staticLength/4.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "";
		productionModule.staticLength = LModule.staticLength/2.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "FAntiClockwise";
		productionModule.staticLength = LModule.staticLength/2.0f;
		productionModule.staticRotation = -DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "";
		productionModule.staticLength = LModule.staticLength/2.0f;
		productionModule.staticRotation = DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "";
		productionModule.staticLength = LModule.staticLength;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FClockwise.weight = 1.0f;
	AddProductionRule("FClockwise", FClockwise);

	ProductionRuleType FAntiClockwise;
	FAntiClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "";
		productionModule.staticLength = LModule.staticLength;
		return productionModule;
	});
	FAntiClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "";
		productionModule.staticLength = LModule.staticLength/2.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
	});
	FAntiClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "FClockwise";
		productionModule.staticLength = LModule.staticLength/2.0f;
		productionModule.staticRotation = DirectX::XM_PI/3.0f;
		return productionModule;
	});
	FAntiClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "";
		productionModule.staticLength = LModule.staticLength/2.0f;
		productionModule.staticRotation = -DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "";
		productionModule.staticLength = LModule.staticLength/4.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
		});
	FAntiClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "FAntiClockwise";
		productionModule.staticLength = LModule.staticLength/2.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
	});
	FAntiClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "";
		productionModule.staticLength = LModule.staticLength/4.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
	});
	FAntiClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "FClockwise";
		productionModule.staticLength = LModule.staticLength/2.0f;
		productionModule.staticRotation = 2.0f*DirectX::XM_PI/3.0f;
		return productionModule;
	});
	FAntiClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "";
		productionModule.staticLength = LModule.staticLength/4.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
	});
	FAntiClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "FClockwise";
		productionModule.staticLength = LModule.staticLength/2.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
	});
	FAntiClockwise.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "";
		productionModule.staticLength = LModule.staticLength/4.0f;
		productionModule.staticRotation = 0.0f;
		return productionModule;
	});
	FAntiClockwise.weight = 1.0f;
	AddProductionRule("FAntiClockwise", FAntiClockwise);

	// STEP 2: Write axiom...
	LModuleType FAntiClockwiseModule = LModuleType();
	FAntiClockwiseModule.letter = "FAntiClockwise";
	FAntiClockwiseModule.staticLength = 2.0f;
	FAntiClockwiseModule.staticWidth = width;

	std::vector<LModuleType> axiom = std::vector<LModuleType>{ FAntiClockwiseModule };

	// STEP 3: Initialise...
	return __super::Initialize(device, axiom, iterations, seed, 0.0f); // NB: Oriented to best 'tend towards a limit'...
}
