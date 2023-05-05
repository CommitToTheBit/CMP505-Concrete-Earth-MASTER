#pragma once

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include <map>
#include <string>
#include <vector>
#include <functional>

class Grammar
{

private:
	struct ProductionRuleType
	{
		std::string production;
		
		//std::function<float(std::string)> context; // FIXME: Expand arguments of context function!

		int dryness;
		float weight;
	};

public:
	Grammar();
	~Grammar();

	void Initialize(std::string jsonPath, float seed = 0.0f);

	// DEBUG:
	void GenerateSentence(std::string axiom);
	std::string GetSentence();

private:
	void AddProductionRule(std::string letter, ProductionRuleType productionRule);

	ProductionRuleType Grammar::GetProductionRule(std::string letter, int turn = -1);
	float GetRNGRange(float a = -1.0f, float b = 1.0f);

private:
	//nlohmann::json m_alphabet;
	std::map<std::string, std::vector<ProductionRuleType>> m_productionRules;
	std::string m_sentence;

	float m_seed;
};

