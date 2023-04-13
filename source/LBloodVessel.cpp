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
		productionModule.staticWidth = LModule.staticWidth/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
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
		productionModule.staticLength = LModule.staticLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = LModule.randomStaticLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = -acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+pow(LModule.bakedAsymmetry, 4.0f)-1.0f)/(2.0f*pow(LModule.bakedAsymmetry, 2.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.staticWidth = LModule.staticWidth*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
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
		productionModule.staticWidth = LModule.staticWidth/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
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
		productionModule.staticLength = LModule.staticLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = LModule.randomStaticLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+pow(LModule.bakedAsymmetry, 4.0f)-1.0f)/(2.0f*pow(LModule.bakedAsymmetry, 2.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.staticWidth = LModule.staticWidth*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
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
	/*LRightBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "[";
		return productionModule;
		});*/
	LRightBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "C";
		productionModule.staticLength = LModule.staticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = LModule.randomStaticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = -acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+1.0f-pow(LModule.bakedAsymmetry, 4.0f))/(2.0f*pow(LModule.bakedAsymmetry, 0.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.staticWidth = LModule.staticWidth/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		return productionModule;
		});
	/*LRightBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "]";
		return productionModule;
		});
	LRightBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "L";
		productionModule.staticLength = LModule.staticLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = LModule.randomStaticLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+pow(LModule.bakedAsymmetry, 4.0f)-1.0f)/(2.0f*pow(LModule.bakedAsymmetry, 2.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.staticWidth = LModule.staticWidth*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		return productionModule;
		});*/
	LRightBifurcation.weight = 1.0f;
	AddProductionRule("L", LRightBifurcation);

	ProductionRuleType RLeftBifurcation;
	RLeftBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "X";
		return productionModule;
		});
	/*RLeftBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "[";
		return productionModule;
		});*/
	RLeftBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "C";
		productionModule.staticLength = LModule.staticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = LModule.randomStaticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+1.0f-pow(LModule.bakedAsymmetry, 4.0f))/(2.0f*pow(LModule.bakedAsymmetry, 0.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.staticWidth = LModule.staticWidth/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		return productionModule;
		});
	/*RLeftBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "]";
		return productionModule;
		});
	RLeftBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "R";
		productionModule.staticLength = LModule.staticLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = LModule.randomStaticLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = -acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+pow(LModule.bakedAsymmetry, 4.0f)-1.0f)/(2.0f*pow(LModule.bakedAsymmetry, 2.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.staticWidth = LModule.staticWidth*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		return productionModule;
		});*/
	RLeftBifurcation.weight = 1.0f;
	AddProductionRule("R", RLeftBifurcation);

	ProductionRuleType X;
	X.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "X";
		productionModule.staticLength = 0.5f*LModule.staticLength;
		productionModule.randomStaticLength = 0.5f*LModule.randomStaticLength;
		productionModule.randomStaticRotation = pow(2.0f,-0.5f)*LModule.randomStaticRotation;
		productionModule.randomPeriodicRotation = pow(2.0f,-0.5f)*LModule.randomPeriodicRotation;
		return productionModule;
		});
	X.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "X";
		productionModule.staticLength = 0.5f*LModule.staticLength;
		productionModule.randomStaticLength = 0.5f*LModule.randomStaticLength;
		productionModule.staticRotation = 0.0f;
		productionModule.randomStaticRotation = pow(2.0f,-0.5f)*LModule.randomStaticRotation;
		productionModule.randomPeriodicRotation = pow(2.0f,-0.5f)*LModule.randomPeriodicRotation;
		return productionModule;
		});
	X.weight = 1.0f;
	AddProductionRule("X", X);

	// STEP 2: Write axiom...
	LModuleType CModule;
	CModule.letter = "C";
	CModule.period = 10.0f;
	CModule.aperiodicity = 10.0f;
	CModule.synchronisation = 0.0f;
	CModule.asynchronicity = 1.0f;
	CModule.staticLength = 1.0f;
	CModule.randomStaticLength = 0.4f;
	CModule.staticRotation = 0.0f;
	CModule.randomStaticRotation = 25.0f*DirectX::XM_PI/180.0f;
	CModule.randomPeriodicRotation = 5.0f*DirectX::XM_PI/180.0f;
	CModule.staticWidth = width;
	CModule.staticAsymmetry = 0.9f;
	CModule.randomStaticAsymmetry = 0.1f;

	std::vector<LModuleType> axiom = std::vector<LModuleType>{ CModule };

	// STEP 3: Initialise...
	return __super::Initialize(device, axiom, iterations, seed, DirectX::XM_PIDIV2); // NB: Oriented to best 'tend towards a limit'...
}
