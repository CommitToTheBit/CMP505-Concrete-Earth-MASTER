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
	ProductionRuleType CStep;
	CStep.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "X";
		return productionModule;
		});
	CStep.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "C";
		productionModule.staticRotation = 0.0f;
		productionModule.staticWidth = 0.95f*LModule.staticWidth/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		return productionModule;
		});
	CStep.weight = 1.0f;
	AddProductionRule("C", CStep);

	ProductionRuleType CLeftBifurcation;
	CLeftBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "X";
		return productionModule;
		});
	CLeftBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "[";
		return productionModule;
		});
	CLeftBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "L";
		productionModule.staticLength = LModule.staticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = LModule.randomStaticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+1.0f-pow(LModule.bakedAsymmetry, 4.0f))/(2.0f*pow(LModule.bakedAsymmetry, 0.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.staticWidth = 0.95f*LModule.staticWidth/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		return productionModule;
		});
	CLeftBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "]";
		return productionModule;
		});
	CLeftBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "R";
		productionModule.staticLength = pow(0.95f, 2.0f)*LModule.staticLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = pow(0.95f, 2.0f)*LModule.randomStaticLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = -0.625f*acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+pow(LModule.bakedAsymmetry, 4.0f)-1.0f)/(2.0f*pow(LModule.bakedAsymmetry, 2.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.staticWidth = pow(0.95f,2.0f)*LModule.staticWidth*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		return productionModule;
		});
	CLeftBifurcation.weight = 1.0f;
	AddProductionRule("C", CLeftBifurcation);

	ProductionRuleType CRightBifurcation;
	CRightBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "X";
		return productionModule;
		});
	CRightBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "[";
		return productionModule;
		});
	CRightBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "R";
		productionModule.staticLength = LModule.staticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = LModule.randomStaticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = -acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+1.0f-pow(LModule.bakedAsymmetry, 4.0f))/(2.0f*pow(LModule.bakedAsymmetry, 0.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.staticWidth = 0.95f*LModule.staticWidth/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		return productionModule;
		});
	CRightBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "]";
		return productionModule;
		});
	CRightBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "L";
		productionModule.staticLength = pow(0.95f, 2.0f)*LModule.staticLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = pow(0.95f, 2.0f)*LModule.randomStaticLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = 0.625f*acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+pow(LModule.bakedAsymmetry, 4.0f)-1.0f)/(2.0f*pow(LModule.bakedAsymmetry, 2.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.staticWidth = pow(0.95f, 2.0f)*LModule.staticWidth*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		return productionModule;
		});
	CRightBifurcation.weight = 1.0f;
	AddProductionRule("C", CRightBifurcation);

	ProductionRuleType LRightBifurcation;
	LRightBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "X";
		return productionModule;
		});
	LRightBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "C";
		productionModule.staticLength = LModule.staticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = LModule.randomStaticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = -0.5f*acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+1.0f-pow(LModule.bakedAsymmetry, 4.0f))/(2.0f*pow(LModule.bakedAsymmetry, 0.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.staticWidth = 0.95f*LModule.staticWidth/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		return productionModule;
		});
	LRightBifurcation.weight = 1.0f;
	AddProductionRule("L", LRightBifurcation);

	ProductionRuleType RLeftBifurcation;
	RLeftBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "X";
		return productionModule;
		});
	RLeftBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "C";
		productionModule.staticLength = LModule.staticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = LModule.randomStaticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = 0.5f*acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+1.0f-pow(LModule.bakedAsymmetry, 4.0f))/(2.0f*pow(LModule.bakedAsymmetry, 0.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.staticWidth = 0.95f*LModule.staticWidth/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		return productionModule;
		});
	RLeftBifurcation.weight = 1.0f;
	AddProductionRule("R", RLeftBifurcation);

	ProductionRuleType X;
	X.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "Y";
		productionModule.staticLength = 0.25f*LModule.staticLength;
		productionModule.randomStaticLength = 0.25f*LModule.randomStaticLength;
		productionModule.randomStaticRotation = pow(0.25f, 0.5f)*LModule.randomStaticRotation;
		productionModule.randomPeriodicRotation = pow(0.25f, 0.5f)*LModule.randomPeriodicRotation;
		return productionModule;
		});
	X.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "Y";
		productionModule.staticLength = 0.25f*LModule.staticLength;
		productionModule.randomStaticLength = 0.25f*LModule.randomStaticLength;
		productionModule.staticRotation = 0.0f;
		productionModule.randomStaticRotation = pow(0.25f, 0.5f)*LModule.randomStaticRotation;
		productionModule.randomPeriodicRotation = pow(0.25f, 0.5f)*LModule.randomPeriodicRotation;
		return productionModule;
		});
	X.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "Y";
		productionModule.staticLength = 0.25f*LModule.staticLength;
		productionModule.randomStaticLength = 0.25f*LModule.randomStaticLength;
		productionModule.staticRotation = 0.0f;
		productionModule.randomStaticRotation = pow(0.25f, 0.5f)*LModule.randomStaticRotation;
		productionModule.randomPeriodicRotation = pow(0.25f, 0.5f)*LModule.randomPeriodicRotation;
		return productionModule;
		});
	X.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "Y";
		productionModule.staticLength = 0.25f*LModule.staticLength;
		productionModule.randomStaticLength = 0.25f*LModule.randomStaticLength;
		productionModule.staticRotation = 0.0f;
		productionModule.randomStaticRotation = pow(0.25f, 0.5f)*LModule.randomStaticRotation;
		productionModule.randomPeriodicRotation = pow(0.25f, 0.5f)*LModule.randomPeriodicRotation;
		return productionModule;
		});
	X.weight = 1.0f;
	AddProductionRule("X", X);

	// STEP 2: Write axiom...
	LModuleType CModule;
	CModule.letter = "C";
	CModule.period = 45.0f;
	CModule.aperiodicity = 45.0f;
	CModule.synchronisation = 0.0f;
	CModule.asynchronicity = 1.0f;
	CModule.staticLength = 1.0f;
	CModule.randomStaticLength = 0.4f;
	CModule.staticRotation = 0.0f;
	CModule.randomStaticRotation = 5.0f*DirectX::XM_PI/180.0f;
	CModule.randomPeriodicRotation = 15.0f*DirectX::XM_PI/180.0f;
	CModule.staticWidth = width;
	CModule.staticAsymmetry = 0.8f;
	CModule.randomStaticAsymmetry = 0.1f;

	std::vector<LModuleType> axiom = std::vector<LModuleType>{ CModule };

	// STEP 3: Initialise...
	return __super::Initialize(device, axiom, iterations, seed, DirectX::XM_PIDIV2); // NB: Oriented to best 'tend towards a limit'...
}
