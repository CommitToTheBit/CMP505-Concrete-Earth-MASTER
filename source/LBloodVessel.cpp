#include "pch.h"
#include "LBloodVessel.h"

#define TRUNK_LENGTH_DAMPING 0.95f;
#define BRANCH_LENGTH_DAMPING 0.5625;
#define BRANCH_ANGLE_DAMPING 1.25f;
#define JOINTS 1;

LBloodVessel::LBloodVessel()
{

}


LBloodVessel::~LBloodVessel()
{

}

bool LBloodVessel::Initialize(ID3D11Device* device, float width, int iterations, float seed)
{
	int joints = JOINTS;

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
		productionModule.staticWidth = LModule.staticWidth/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		return productionModule;
		});
	CStep.weight = 0.0f;
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
		float trunkLengthDamping = TRUNK_LENGTH_DAMPING;

		productionModule.letter = "L";
		productionModule.staticLength = trunkLengthDamping*LModule.staticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = trunkLengthDamping*LModule.randomStaticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomPeriodicLength = trunkLengthDamping*LModule.randomPeriodicLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+1.0f-pow(LModule.bakedAsymmetry, 4.0f))/(2.0f*pow(LModule.bakedAsymmetry, 0.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.periodicRotation = (1.0f+1.0f/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f))*LModule.periodicRotation;
		productionModule.randomPeriodicRotation = (1.0f+1.0f/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f))*LModule.randomPeriodicRotation;
		productionModule.staticWidth = trunkLengthDamping*LModule.staticWidth/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		return productionModule;
		});
	CLeftBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "]";
		return productionModule;
		});
	CLeftBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		float branchLengthDamping = BRANCH_LENGTH_DAMPING;

		productionModule.letter = "R";
		productionModule.staticLength = branchLengthDamping*LModule.staticLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = branchLengthDamping*LModule.randomStaticLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomPeriodicLength = branchLengthDamping*LModule.randomPeriodicLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = -acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+pow(LModule.bakedAsymmetry, 4.0f)-1.0f)/(2.0f*pow(LModule.bakedAsymmetry, 2.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.periodicRotation = (1.0f+1.0f*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f))*LModule.periodicRotation; 
		productionModule.randomPeriodicRotation = (1.0f+1.0f*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f))*LModule.randomPeriodicRotation;
		productionModule.staticWidth = branchLengthDamping*LModule.staticWidth*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
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
		float trunkLengthDamping = TRUNK_LENGTH_DAMPING;

		productionModule.letter = "R";
		productionModule.staticLength = trunkLengthDamping*LModule.staticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = trunkLengthDamping*LModule.randomStaticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomPeriodicLength = trunkLengthDamping*LModule.randomPeriodicLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = -acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+1.0f-pow(LModule.bakedAsymmetry, 4.0f))/(2.0f*pow(LModule.bakedAsymmetry, 0.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.randomPeriodicRotation = (1.0f+1.0f/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f))*LModule.randomPeriodicRotation;
		productionModule.periodicRotation = (1.0f+1.0f/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f))*LModule.periodicRotation;
		productionModule.staticWidth = trunkLengthDamping*LModule.staticWidth/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		return productionModule;
		});
	CRightBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		productionModule.letter = "]";
		return productionModule;
		});
	CRightBifurcation.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		float branchLengthDamping = BRANCH_LENGTH_DAMPING;

		productionModule.letter = "L";
		productionModule.staticLength = branchLengthDamping*LModule.staticLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = branchLengthDamping*LModule.randomStaticLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomPeriodicLength = branchLengthDamping*LModule.randomPeriodicLength*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+pow(LModule.bakedAsymmetry, 4.0f)-1.0f)/(2.0f*pow(LModule.bakedAsymmetry, 2.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.periodicRotation = (1.0f+1.0f*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f))*LModule.periodicRotation;
		productionModule.randomPeriodicRotation = (1.0f+1.0f*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f))*LModule.randomPeriodicRotation;
		productionModule.staticWidth = branchLengthDamping*LModule.staticWidth*LModule.bakedAsymmetry/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
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
		float trunkLengthDamping = TRUNK_LENGTH_DAMPING;
		float branchAngleDamping = BRANCH_ANGLE_DAMPING;

		productionModule.letter = "C";
		productionModule.staticLength = trunkLengthDamping*LModule.staticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = trunkLengthDamping*LModule.randomStaticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomPeriodicLength = trunkLengthDamping*LModule.randomPeriodicLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = -branchAngleDamping*acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+1.0f-pow(LModule.bakedAsymmetry, 4.0f))/(2.0f*pow(LModule.bakedAsymmetry, 0.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.staticWidth = trunkLengthDamping*LModule.staticWidth/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
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
		float trunkLengthDamping = TRUNK_LENGTH_DAMPING;
		float branchAngleDamping = BRANCH_ANGLE_DAMPING;

		productionModule.letter = "C";
		productionModule.staticLength = trunkLengthDamping*LModule.staticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomStaticLength = trunkLengthDamping*LModule.randomStaticLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.randomPeriodicLength = trunkLengthDamping*LModule.randomPeriodicLength/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		productionModule.staticRotation = branchAngleDamping*acos((pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 4.0f/3.0f)+1.0f-pow(LModule.bakedAsymmetry, 4.0f))/(2.0f*pow(LModule.bakedAsymmetry, 0.0f)*pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 2.0f/3.0f)));
		productionModule.staticWidth = trunkLengthDamping*LModule.staticWidth/pow(1.0f+pow(LModule.bakedAsymmetry, 3.0f), 1.0f/3.0f);
		return productionModule;
		});
	RLeftBifurcation.weight = 1.0f;
	AddProductionRule("R", RLeftBifurcation);

	ProductionRuleType X;
	X.productions.push_back([](LModuleType LModule) {
		LModuleType productionModule = LModule;
		int joints = JOINTS;

		productionModule.letter = "Y";
		productionModule.staticLength = (1.0f/joints)*LModule.staticLength;
		productionModule.randomStaticLength = (1.0f/joints)*LModule.randomStaticLength;
		productionModule.randomPeriodicLength = (1.0f/joints)*LModule.randomPeriodicLength;
		productionModule.randomStaticRotation = (1.0f/joints)*LModule.randomStaticRotation;
		productionModule.periodicRotation = (1.0f/joints)*LModule.periodicRotation;
		productionModule.randomPeriodicRotation = (1.0f/joints)*LModule.randomPeriodicRotation;
		return productionModule;
		});
	for (int i = 1; i < joints; i++)
	{
		X.productions.push_back([](LModuleType LModule) {
			LModuleType productionModule = LModule;
			int joints = JOINTS;

			productionModule.letter = "Y";
			productionModule.staticLength = (1.0f/joints)*LModule.staticLength;
			productionModule.randomStaticLength = (1.0f/joints)*LModule.randomStaticLength;
			productionModule.randomPeriodicLength = (1.0f/joints)*LModule.randomPeriodicLength;
			productionModule.staticRotation = 0.0f;
			productionModule.randomStaticRotation = (1.0f/joints)*LModule.randomStaticRotation;
			productionModule.periodicRotation = (1.0f/joints)*LModule.periodicRotation;
			productionModule.randomPeriodicRotation = (1.0f/joints)*LModule.randomPeriodicRotation;
			return productionModule;
			});
	}
	X.weight = 1.0f;
	AddProductionRule("X", X);

	// STEP 2: Write axiom...
	LModuleType CModule;
	CModule.letter = "C";
	CModule.period = 2.5f;
	CModule.aperiodicity = 0.5f;
	CModule.synchronisation = 0.0f;
	CModule.asynchronicity = 1.0f;
	CModule.staticLength = 1.0f;
	CModule.randomStaticLength = 0.5f;
	CModule.randomPeriodicLength = 0.125f;
	CModule.staticRotation = 0.0f;
	CModule.randomStaticRotation = 4.0f*DirectX::XM_PI/180.0f;
	CModule.periodicRotation = 0.1f*DirectX::XM_PI/180.0f;
	CModule.randomPeriodicRotation = 1.0f*DirectX::XM_PI/180.0f;
	CModule.staticWidth = width;
	CModule.staticAsymmetry = 0.8f;
	CModule.randomStaticAsymmetry = 0.1f;

	std::vector<LModuleType> axiom = std::vector<LModuleType>{ CModule };

	// STEP 3: Initialise...
	return __super::Initialize(device, axiom, iterations, seed, DirectX::XM_PIDIV2); // NB: Oriented to best 'tend towards a limit'...
}
